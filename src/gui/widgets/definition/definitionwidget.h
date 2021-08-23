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

#ifndef DEFINITIONWIDGET_H
#define DEFINITIONWIDGET_H

#include <QWidget>

#include "../../../dict/expression.h"
#include "../../../anki/ankiclient.h"
#include "termwidget.h"

#include <QWheelEvent>

namespace Ui
{
    class DefinitionWidget;
}

class DefinitionWidget : public QWidget
{
    Q_OBJECT

public:
    DefinitionWidget(const QList<SubtitleExtract *> *terms, QWidget *parent = 0);
    ~DefinitionWidget();

private Q_SLOTS:
    void setTheme();
    void setAddable(const size_t start, const size_t end);
    void showTerms(const size_t start, const size_t end);
    void showKanji(const SubtitleExtract *kanji);
    void hideKanji();

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

private:
    Ui::DefinitionWidget *m_ui;
    AnkiClient           *m_client;
    QList<AudioSource>    m_sources;
    const QList<SubtitleExtract *>  *m_terms;
    QList<bool>           m_addable;
    QList<TermWidget *>   m_termWidgets;
    int                   m_savedScroll;
};

#endif // DEFINITIONWIDGET_H