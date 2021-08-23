//
// Created by user on 7/3/21.
//

#ifndef MEMENTO_DICTREADER_H
#define MEMENTO_DICTREADER_H

#include <QApplication>
#include <zlib.h>
#include "expression.h"
#include "dictionary.h"

class DictReader {

public:
    DictReader(QString filename);
    ~DictReader();
    void readIntoDictionary(Dictionary *dict);

private:
    void readBytes(int len);
    uint32_t readUInt32();
    uint8_t readUInt8();
    QString readString();
    QList<QString> readStrings();
    SyntaxInfo readSyntaxInfo();
    QList<SyntaxInfo> readSyntaxInfos();

    gzFile file;
    uint8_t *buf;

};


#endif //MEMENTO_DICTREADER_H
