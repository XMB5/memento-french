////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "subtitlewidget.h"

#include "../../util/directoryutils.h"
#include "../../util/globalmediator.h"
#include "../../util/constants.h"

#include "../playeradapter.h"

#include "../../dict/frenchprocessor.h"

#include <QApplication>
#include <QClipboard>
#include <QThreadPool>
#include <QDebug>
#include <QScrollBar>
#include <QSettings>
#include <QTextLayout>
#include <QPainter>
#include <QPainterPath>

#define BORDER_SIZE 4
#define DOUBLE_DELTA            0.05

SubtitleWidget::SubtitleWidget(QWidget *parent) : QWidget(parent),
                                                  m_paused(true),
                                                  m_currentIndex(-1)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setAcceptDrops(false);
    hide();
    setCursor(Qt::ArrowCursor);
    setMouseTracking(true);

    changeFont();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Slots */
    connect(mediator, &GlobalMediator::playerResized, this, &SubtitleWidget::onPlayerResize);
    connect(mediator,    &GlobalMediator::playerSubtitleChanged,      this, &SubtitleWidget::setSubtitle);
    connect(mediator,    &GlobalMediator::playerPositionChanged,      this, &SubtitleWidget::positionChanged);
    connect(mediator,    &GlobalMediator::playerSubtitlesDisabled,    this, [=] { positionChanged(-1); } );
    connect(mediator,    &GlobalMediator::playerSubtitleTrackChanged, this, [=] { positionChanged(-1); } );
    connect(mediator,    &GlobalMediator::playerPauseStateChanged,    this, 
        [=] (const bool paused) {
            m_paused = paused;
            adjustVisibility();
        }
    );
}

SubtitleWidget::~SubtitleWidget()
{
    disconnect();
}

void SubtitleWidget::adjustVisibility()
{
    if (m_rawText.isEmpty())
    {
        hide();
    }
    else if (m_paused)
    {
        show();
    }
    else
    {
        show();
    }
}

void SubtitleWidget::setSubtitle(QString subtitle,
                                 const double start, 
                                 const double end,
                                 const double delay)
{
    this->m_rawText = subtitle;
    this->subtitleInfo = GlobalMediator::getGlobalMediator()->getFrenchProcessor()->processSubtitle(subtitle);

    loadTextLayout();
    fitToContents();
    update();

    /* Keep track of when to delete the subtitle */
    m_startTime = start + delay;
    m_endTime = end + delay;

    adjustVisibility();
}

void SubtitleWidget::positionChanged(const double value)
{
    if (value < m_startTime - DOUBLE_DELTA || value > m_endTime + DOUBLE_DELTA)
    {
        m_rawText = "";
        hide();
        Q_EMIT GlobalMediator::getGlobalMediator()->subtitleExpired();
    }
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_paused) {
        for (int i = 0; i < this->charBoundaries.size(); i++) {
            QRectF &charBoundary = this->charBoundaries[i];
            if (charBoundary.contains(event->pos())) {
                if (i != m_currentIndex) {
                    //TODO check if same word?
                    for (SubtitlePhrase &phrase : this->subtitleInfo.phrases) {
                        if (i >= phrase.start && i < phrase.stop) {
                            auto *terms = new QList<SubtitleExtract*>;
                            auto *extract = new SubtitleExtract;
                            extract->subtitleText = this->m_rawText;
                            extract->phrase = phrase;
                            terms->append(extract);
                            Q_EMIT GlobalMediator::getGlobalMediator()->termsChanged(terms);
                        }
                    }

                    m_currentIndex = i;
                }
                break;
            }
        }
    }
}

void SubtitleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QApplication::clipboard()->setText(m_rawText);
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    qDebug() << "leave event";
    QWidget::leaveEvent(event);
}

void SubtitleWidget::paintEvent(QPaintEvent *event)
{
    QElapsedTimer timer;
    timer.start();

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    this->charBoundaries.clear();

    for (std::unique_ptr<QTextLayout> &textLayout : this->textLayouts) {
        for (int lineNum = 0; lineNum < textLayout->lineCount(); lineNum++) {
            QTextLine line = textLayout->lineAt(lineNum);
            for (int layoutCharNum = line.textStart(); layoutCharNum < line.textStart() + line.textLength(); layoutCharNum++) {
                QList<QGlyphRun> glyphRuns = line.glyphRuns(layoutCharNum, 1);
                int numGlyphRuns = glyphRuns.length();
                if (numGlyphRuns == 0) {
                    // spaces at the end of a line do not have any glyph
                    charBoundaries.emplace_back();
                    continue;
                } else if (numGlyphRuns > 1) {
                    throw std::runtime_error("expected only 1 glyphRuns");
                }

                QGlyphRun glyphRun = glyphRuns.front();
                if (glyphRun.positions().length() != 1) {
                    throw std::runtime_error("expected only 1 glyph");
                }

                int charNum = this->charBoundaries.size();
                if (charNum == this->subtitleInfo.charColors.size()) {
                    throw std::runtime_error("not enough char colors");
                }
                SubtitleCharColors colors = this->subtitleInfo.charColors[charNum];

                quint32 glyphIndex = glyphRun.glyphIndexes()[0];
                QPointF position = glyphRun.positions()[0];
                QRawFont rawFont = glyphRun.rawFont();

                QPainterPath path = rawFont.pathForGlyph(glyphIndex);
                path.translate(BORDER_SIZE, BORDER_SIZE);
                path.translate(position);
                charBoundaries.push_back(path.boundingRect());

                if (!path.isEmpty()) {
                    // for ligatures (like fi or ff in some fonts) the first char will be empty, and the second char will have the glyph
                    painter.strokePath(path, QPen(QBrush(colors.bgColor), BORDER_SIZE * 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.fillPath(path, QBrush(colors.fgColor));
                }
            }
        }

        if (textLayout != this->textLayouts.back()) {
            // skip over the '\n' character
            charBoundaries.emplace_back();
        }
    }

    if (charBoundaries.size() != this->subtitleInfo.charColors.size()) {
        throw std::runtime_error("did not use all char colors");
    }

    qDebug() << "text render" << timer.elapsed();
}

void SubtitleWidget::showEvent(QShowEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->requestSetSubtitleVisibility(false);
    QWidget::showEvent(event);
}

void SubtitleWidget::hideEvent(QHideEvent *event)
{
    //TODO: options like in original
    //Q_EMIT GlobalMediator::getGlobalMediator()->requestSetSubtitleVisibility(true);
    QWidget::hideEvent(event);
}

void SubtitleWidget::changeFont() {
    int fontSize = GlobalMediator::getGlobalMediator()->getPlayerWidget()->height() * 55 / 1080;
    if (this->font().pointSize() != fontSize || this->font().family() != SETTINGS_INTERFACE_SUB_FONT_DEFAULT) {
        qDebug() << "set font size to" << fontSize;
        this->setFont(QFont(SETTINGS_INTERFACE_SUB_FONT_DEFAULT, fontSize));
    }
}

void SubtitleWidget::loadTextLayout() {
    this->textLayouts.clear();

    int playerWidth = GlobalMediator::getGlobalMediator()->getPlayerWidget()->width();
    int lineWidth = playerWidth - BORDER_SIZE * 2;
    int leading = this->fontMetrics().leading();

    qreal height = 0;

    QStringList lines = this->m_rawText.split('\n');
    for (QString &lineStr : lines) {
        // we need a new textLayout for each line because QTextLayout ignores '\n'
        std::unique_ptr<QTextLayout> textLayout = std::make_unique<QTextLayout>(lineStr, this->font());
        textLayout->setCacheEnabled(true);
        QTextOption textOption;
        textOption.setAlignment(Qt::AlignHCenter);
        textLayout->setTextOption(textOption);

        textLayout->beginLayout();
        while (true) {
            QTextLine textLine = textLayout->createLine();
            if (!textLine.isValid())
                // used up all characters
                break;

            textLine.setLineWidth(lineWidth);
            height += leading;
            textLine.setPosition(QPointF(0, height));
            height += textLine.height();
        }
        textLayout->endLayout();

        this->textLayouts.push_back(std::move(textLayout));
    }
}

void SubtitleWidget::fitToContents() {
    // get bounding rect around all textLayouts
    QRect boundingRect;
    bool first = true;
    for (std::unique_ptr<QTextLayout> &layout : this->textLayouts) {
        if (first) {
            boundingRect = layout->boundingRect().toRect();
            first = false;
        } else {
            boundingRect = boundingRect.united(layout->boundingRect().toRect());
        }
    }

    QSize size = boundingRect.size() + QSize(BORDER_SIZE * 2, BORDER_SIZE * 2);
    setFixedSize(size);
    updateGeometry();
}

void SubtitleWidget::onPlayerResize() {
    this->changeFont();
    this->loadTextLayout();
    this->fitToContents();
    this->update();

    Q_EMIT GlobalMediator::getGlobalMediator()->requestDefinitionDelete();
}
