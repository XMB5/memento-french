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

#include "dictionary.h"

#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <algorithm>
#include <QFile>

#include "../util/directoryutils.h"
#include "dictreader.h"
#include "frenchprocessor.h"

Dictionary::Dictionary() {
    this->loadDict(DirectoryUtils::getDictionaryFile());
    this->loadCss(DirectoryUtils::getDictionaryCssFile());
}

Dictionary::~Dictionary()
{
    for (QHash<QString, DictEntry*>::iterator pair = this->map.begin(); pair != this->map.end(); pair++) {
        delete pair.value();
    }
}

void Dictionary::loadDict(const QString filename) {
    qDebug() << "load dictionary from" << filename;
    DictReader reader{filename};
    reader.readIntoDictionary(this);
}

void Dictionary::loadCss(const QString filename) {
    qDebug() << "load css from" << filename;
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        throw std::runtime_error("failed to open css file");
    }
    this->termCss = file.readAll();
}