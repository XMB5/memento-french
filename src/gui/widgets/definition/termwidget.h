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

#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <QWidget>
#include <QMouseEvent>

#include "../common/flowlayout.h"
#include "../../../dict/expression.h"
#include "../../../anki/ankiclient.h"

namespace Ui
{
    class TermWidget;
}

class TermWidget : public QWidget
{
    Q_OBJECT

public:
    TermWidget(QWidget *parent = nullptr);
    ~TermWidget();

    void setAddable(bool value);
    void setTerm(SubtitleExtract *term);

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;
    /* Prevents these events from being sent to mpv when widget has focus */
    void mouseMoveEvent(QMouseEvent *event) override
    { event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) override
    { event->accept(); }
    void mouseDoubleClickEvent(QMouseEvent *event) override
    { event->accept(); }
    void mousePressEvent(QMouseEvent *event) override
    { event->accept(); }
    void wheelEvent(QWheelEvent *event) override
    { event->accept(); }

private Q_SLOTS:
    void addNote();
    void openAnki();
    void playAudio(QString lang, QString tld, bool slow);
    void showAudioSources(const QPoint &pos);

private:
    Ui::TermWidget *m_ui;
    SubtitleExtract *m_term;

};

#endif // TERMWIDGET_H