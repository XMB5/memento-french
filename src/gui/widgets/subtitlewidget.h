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

#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include "../../dict/expression.h"

#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QTextLayout>

#include <vector>

class SubtitleWidget : public QWidget
{
    Q_OBJECT

public:
    SubtitleWidget(QWidget *parent = 0);
    ~SubtitleWidget();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private Q_SLOTS:
    void adjustVisibility();
    void positionChanged(const double value);
    void setSubtitle(QString subtitle,
                     const double start,
                     const double end,
                     const double delay);
    void onPlayerResize();

private:
    int         m_currentIndex;
    bool        m_paused;

    SubtitleInfo subtitleInfo;
    QString     m_rawText;
    double      m_startTime;
    double      m_endTime;

    std::vector<std::unique_ptr<QTextLayout>> textLayouts;
    std::vector<QRectF> charBoundaries;

    void changeFont();
    void loadTextLayout();
    void fitToContents();
};

#endif // SUBTITLEWIDGET_H