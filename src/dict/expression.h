////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QString>
#include <QColor>
#include <QList>

struct SyntaxInfo {
    QString partOfSpeech;
    QString lemma;
    QString morphosyntacticTag;
};

struct DictEntry {
    QList<SyntaxInfo> syntaxInfos;
    QList<QString> definitions;
};

struct SubtitleCharColors {
    QColor fgColor;
    QColor bgColor;
};

struct SubtitlePhrase {
    int start;
    int stop;
    DictEntry *dictEntry;
};

struct SubtitleExtract {
    QString subtitleText;
    SubtitlePhrase phrase;
};

struct SubtitleInfo {
    std::vector<SubtitleCharColors> charColors;
    std::vector<SubtitlePhrase> phrases;
};

#endif // EXPRESSION_H