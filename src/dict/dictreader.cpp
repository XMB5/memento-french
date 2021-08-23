//
// Created by user on 7/3/21.
//

#include "dictreader.h"

#include <QDebug>
#include <QtEndian>
#include <zlib.h>
#include <cerrno>

static const size_t BUF_SIZE = 131072;

DictReader::DictReader(const QString filename) {
    this->buf = new uint8_t[BUF_SIZE];

    std::string fileStr = filename.toStdString();
    errno = 0;
    this->file = gzopen(fileStr.c_str(), "rb");
    if (this->file == Z_NULL) {
        if (errno == 0) {
            throw std::runtime_error("failed to open file " + fileStr);
        } else {
            throw std::system_error(errno, std::generic_category(), fileStr);
        }
    }
}

DictReader::~DictReader() {
    delete[] this->buf;
    if (this->file != Z_NULL) {
        if (gzclose(this->file) != Z_OK) {
            qDebug() << "failed to close gzip file";
        }
    }
}

void DictReader::readBytes(int len) {
    if (len > BUF_SIZE) {
        throw std::runtime_error("read length " + std::to_string(len) + " greater than maximum of " + std::to_string(BUF_SIZE));
    }

    int readLen = 0;
    while (readLen < len) {
        int gzResponse = gzread(this->file, this->buf + readLen, len - readLen);
        if (gzResponse > 0) {
            readLen += gzResponse;
        } else if (gzResponse == 0 && gzeof(this->file)) {
            throw std::runtime_error("gzip eof before end of data");
        } else {
            int errnum;
            const char* errStr = gzerror(this->file, &errnum);
            throw std::runtime_error(errStr);
        }
    }
}

uint32_t DictReader::readUInt32() {
    this->readBytes(4);
    return qFromLittleEndian(*((uint32_t*) this->buf));
}

uint8_t DictReader::readUInt8() {
    this->readBytes(1);
    return *((uint8_t*) this->buf);
}

QString DictReader::readString() {
    int len = this->readUInt32();
    this->readBytes(len);
    QByteArray bytes = QByteArray::fromRawData((const char*) this->buf, len);
    return QString(bytes);
}

QList<QString> DictReader::readStrings() {
    QList<QString> strings;
    int numStrings = this->readUInt8();
    for (int i = 0; i < numStrings; i++) {
        strings.append(this->readString());
    }
    return strings;
}

SyntaxInfo DictReader::readSyntaxInfo() {
    SyntaxInfo info;
    info.partOfSpeech = this->readString();
    info.lemma = this->readString();
    info.morphosyntacticTag = this->readString();
    return info;
}

QList<SyntaxInfo> DictReader::readSyntaxInfos() {
    QList<SyntaxInfo> infos;
    int len = this->readUInt8();
    for (int i = 0; i < len; i++) {
        infos.append(this->readSyntaxInfo());
    }
    return infos;
}

void DictReader::readIntoDictionary(Dictionary *dict) {
    int numEntries = this->readUInt32();
    dict->map.reserve(numEntries);
    for (int i = 0; i < numEntries; i++) {
        QString word = this->readString();
        DictEntry *entry = new DictEntry;
        entry->syntaxInfos = this->readSyntaxInfos();
        entry->definitions = this->readStrings();
        dict->map[word] = entry;
    }
}
