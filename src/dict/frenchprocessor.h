//
// Created by user on 7/4/21.
//

#ifndef MEMENTO_FRENCHPROCESSOR_H
#define MEMENTO_FRENCHPROCESSOR_H

#include <QString>
#include "expression.h"

class FrenchProcessor {

public:
    SubtitleInfo processSubtitle(QString rawText);

private:
    QString cleanWord(QString word);

};


#endif //MEMENTO_FRENCHPROCESSOR_H
