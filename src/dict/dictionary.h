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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QString>
#include <QList>
#include <QThread>
#include <QHash>
#include "expression.h"

class Dictionary
{
public:
    Dictionary();
    ~Dictionary();

    QHash<QString,DictEntry*> map;
    QString termCss;

private:
    void loadDict(QString filename);
    void loadCss(QString filename);

};

#endif // DICTIONARY_H