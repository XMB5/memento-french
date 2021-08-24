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

#include "termwidget.h"
#include "ui_termwidget.h"

#include "../../../util/iconfactory.h"
#include "../../../util/globalmediator.h"
#include "../../../dict/dictionary.h"
#include "../../../audio/audioplayer.h"

#include <QMenu>
#include <utility>

#define KANJI_STYLE_STRING      (QString("<style>a { color: %1; border: 0; text-decoration: none; }</style>"))
#define KANJI_FORMAT_STRING     (QString("<a href=\"%1\">%1</a>"))

#if __APPLE__
    #define EXPRESSION_STYLE    (QString("QLabel { font-size: 30pt; }"))
    #define READING_STYLE       (QString("QLabel { font-size: 18pt; }"))
#else
    #define EXPRESSION_STYLE    (QString("QLabel { font-size: 20pt; }"))
    #define READING_STYLE       (QString("QLabel { font-size: 12pt; }"))
#endif

TermWidget::TermWidget(QWidget *parent) : QWidget(parent), m_ui(new Ui::TermWidget), m_term(nullptr) {
    m_ui->setupUi(this);

    IconFactory *factory = IconFactory::create();

    m_ui->buttonAddCard->setIcon(factory->getIcon(IconFactory::Icon::plus));
    m_ui->buttonAddCard->setVisible(false);

    m_ui->buttonAnkiOpen->setIcon(factory->getIcon(IconFactory::Icon::hamburger));
    m_ui->buttonAnkiOpen->setVisible(false);

    m_ui->buttonAudio->setIcon(factory->getIcon(IconFactory::Icon::audio));
    auto &sources = GlobalMediator::getGlobalMediator()->getAudioPlayer()->audioSources;
    m_ui->buttonAudio->setVisible(!sources.isEmpty());

    connect(m_ui->buttonAddCard,  &QToolButton::clicked,  this, &TermWidget::addNote);
    connect(m_ui->buttonAnkiOpen, &QToolButton::clicked,  this, &TermWidget::openAnki);
    connect(m_ui->buttonAudio,    &QToolButton::clicked,  this, 
        [=] {
            if (!sources.isEmpty())
            {
                const AudioSource &src = sources.first();
                playAudio(src.lang, src.tld, src.slow);
            }
        } 
    );
    connect(m_ui->buttonAudio, &QToolButton::customContextMenuRequested, this, &TermWidget::showAudioSources);
}

TermWidget::~TermWidget()
{
    delete m_term;
    delete m_ui;
}

void TermWidget::setAddable(bool value)
{
    m_ui->buttonAddCard->setVisible(value);
    m_ui->buttonAnkiOpen->setVisible(!value);
}

void TermWidget::addNote()
{
    //TODO add note
//    m_ui->buttonAddCard->setEnabled(false);
//
//    Term *term = new Term(*m_term);
//    term->definitions.clear();
//    for (size_t i = 0; i < m_layoutGlossary->count(); ++i)
//    {
//        GlossaryWidget *widget = (GlossaryWidget *)m_layoutGlossary->itemAt(i)->widget();
//        if (widget->isChecked())
//        {
//            term->definitions.append(TermDefinition(m_term->definitions[i]));
//        }
//        widget->setCheckable(false);
//    }
//
//    AnkiReply *reply = m_client->addNote(term);
//    connect(reply, &AnkiReply::finishedInt, this,
//        [=] (const int id, const QString &error) {
//            if (!error.isEmpty())
//            {
//                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical("Error Adding Note", error);
//            }
//            else
//            {
//                m_ui->buttonAnkiOpen->setVisible(true);
//                m_ui->buttonAddCard->setVisible(false);
//            }
//        }
//    );
}

void TermWidget::openAnki()
{
    //TODO open anki
//    QString deck = GlobalMediator::getGlobalMediator()->getAnkiClient()->getConfig()->termDeck;
//    AnkiReply *reply = GlobalMediator::getGlobalMediator()->getAnkiClient()->openBrowse(deck, m_term->expression);
//    connect(reply, &AnkiReply::finishedIntList, this,
//        [=] (const QList<int> &value, const QString &error) {
//            if (!error.isEmpty())
//            {
//                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
//                    "Error Opening Anki", error
//                );
//            }
//        }
//    );
}

void TermWidget::playAudio(QString lang, QString tld, bool slow)
{
    m_ui->buttonAudio->setEnabled(false);
    QString substring = m_term->subtitleText.mid(m_term->phrase.start, m_term->phrase.stop - m_term->phrase.start);
    AudioPlayerReply *reply = GlobalMediator::getGlobalMediator()->getAudioPlayer()->playAudio(substring, std::move(lang), std::move(tld), slow);
    m_ui->buttonAudio->setEnabled(reply == nullptr);

    if (reply)
    {
        connect(reply, &AudioPlayerReply::result, this,
            [=] (const bool success) {
                if (!success)
                {
                    IconFactory *factory = IconFactory::create();
                    m_ui->buttonAudio->setIcon(factory->getIcon(IconFactory::noaudio));
                }
                m_ui->buttonAudio->setEnabled(true);
            }
        );
    }
}

void TermWidget::showAudioSources(const QPoint &pos)
{
    QMenu contextMenu("Audio Sources", m_ui->buttonAudio);
    for (const AudioSource &src : GlobalMediator::getGlobalMediator()->getAudioPlayer()->audioSources)
    {
        contextMenu.addAction(src.name, this, [=] { playAudio(src.lang, src.tld, src.slow); });
    }
    contextMenu.exec(m_ui->buttonAudio->mapToGlobal(pos));
}

/**
 * Change to a new term
 * @param extract TermWidget takes ownership (will delete when done)
 */
void TermWidget::setTerm(SubtitleExtract *extract) {
    delete this->m_term;
    this->m_term = extract;
    
    QStringRef phraseStr = m_term->subtitleText.midRef(m_term->phrase.start, m_term->phrase.stop - m_term->phrase.start);
    this->m_ui->extractLabel->setText(phraseStr.toString());

    QString html = "<html><head><style>" + GlobalMediator::getGlobalMediator()->getDictionary()->termCss + "</style><body>";

    for (auto &def : m_term->phrase.dictEntry->definitions) {
        html += def;
    }

    // the extract might not have any definitions because the word isn't a lemma
    // we need to check all forms of the word -> find lemmas -> add their definitions
    std::multimap<QString, SyntaxInfo> lemmas;

    for (SyntaxInfo &info: m_term->phrase.dictEntry->syntaxInfos) {
        if (!info.lemma.isEmpty()) {
            lemmas.emplace(info.lemma, info);
        }
    }

    auto it = lemmas.begin();
    auto end = lemmas.end();
    while (it != end) {
        QString lemma = it->first;
        DictEntry *lemmaEntry = lemma.isEmpty() ? m_term->phrase.dictEntry : GlobalMediator::getGlobalMediator()->getDictionary()->map[lemma];

        if (lemmaEntry != nullptr) {
            for (auto &def : lemmaEntry->definitions) {
                html += def;
            }
        }

        it = lemmas.upper_bound(lemma);
    }

    html += "</body></html>";
    this->m_ui->webEngineView->setHtml(html);
}

void TermWidget::hideEvent(QHideEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->definitionsHidden();
}

void TermWidget::showEvent(QShowEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->definitionsShown();
}