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
    TermWidget(const SubtitleExtract    *term,
               const QList<AudioSource> *sources,
               QWidget                  *parent = 0);
    ~TermWidget();

    void setAddable(bool value);

Q_SIGNALS:
    void kanjiSearched(const SubtitleExtract *kanji);

private Q_SLOTS:
    void addNote();
    void openAnki();
    void playAudio(QString url, const QString &hash);
    void showAudioSources(const QPoint &pos);
    void searchKanji(const QString &ch);

private:
    Ui::TermWidget           *m_ui;
    const SubtitleExtract    *m_term;
    const QList<AudioSource> *m_sources;

    void setTerm(const SubtitleExtract &term);
};

#endif // TERMWIDGET_H