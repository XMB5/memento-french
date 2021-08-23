//
// Created by user on 7/4/21.
//

#include "frenchprocessor.h"
#include "../util/globalmediator.h"
#include "dictionary.h"
#include <QDebug>
#include <vector>
#include <tuple>

SubtitleInfo FrenchProcessor::processSubtitle(QString rawText) {
    std::vector<std::tuple<int, int, QString>> words;

    int currentWordStart = 0;
    for (int i = 0; i <= rawText.size(); i++) {
        if (i == rawText.size() || rawText[i].isSpace() || rawText[i] == '-' || rawText[i] == '\'') {
            int wordEnd = i;
            bool includeSeparator = i < rawText.size() && rawText[i] == '\'';
            if (includeSeparator) {
                wordEnd++;
            }

            QString word = QStringRef(&rawText, currentWordStart, wordEnd - currentWordStart).toString();
            word = cleanWord(word);
            // remove punctuation from beginning
            while (word.size() > 0 && word[0].isPunct()) {
                word = word.remove(0, 1);
            }
            // remove punctuatiom from end (when not purposefully including separator)
            if (!includeSeparator) {
                while (word.size() > 0 && word[word.size() - 1].isPunct()) {
                    word = word.remove(word.size() - 1, 1);
                }
            }

            if (word.size() > 0) {
                words.emplace_back(currentWordStart, wordEnd, word);
            }
            currentWordStart = i + 1;
        }
    }

    SubtitleInfo out;

    for (int i = 0; i < words.size();) {
        bool found = false;
        for (int groupSize = std::min((int) words.size() - i, 7); groupSize >= 1; groupSize--) {
            // try searching in the dictionary for phrases up to 7 words long
            // longest dictionary phrase: Maison des jeunes et de la culture
            QString group;
            for (int wordNum = i; wordNum < i + groupSize; wordNum++) {
                auto [start, stop, word] = words[wordNum];

                if (wordNum > i) {
                    // add space or dash between words
                    if (rawText[start - 1] == '-') {
                        group += '-';
                    } else {
                        group += ' ';
                    }
                }
                group += word;
            }

            DictEntry *lookupResult = GlobalMediator::getGlobalMediator()->getDictionary()->map[group];
            if (lookupResult != nullptr) {
                SubtitlePhrase phrase{};
                phrase.start = std::get<0>(words[i]);
                phrase.stop = std::get<1>(words[i + groupSize - 1]);
                phrase.dictEntry = lookupResult;
                out.phrases.push_back(phrase);
                found = true;
                i += groupSize;
                break;
            }
        }
        if (!found) {
            i++;
        }
    }

    for (int i = 0; i < rawText.size(); i++) {
        DictEntry *entry = nullptr;

        for (SubtitlePhrase &phrase : out.phrases) {
            if (i >= phrase.start && i < phrase.stop) {
                entry = phrase.dictEntry;
                break;
            }
        }

        SubtitleCharColors colors;
        colors.bgColor = QColor(0, 0, 0);
        if (entry == nullptr) {
            colors.fgColor = Qt::white;
        } else {
            if (entry->syntaxInfos.isEmpty()) {
                // example word: surendettement
                colors.fgColor = Qt::white;
            } else {
                bool masculine = true;
                bool feminine = true;
                for (SyntaxInfo &info : entry->syntaxInfos) {
                    if (!info.morphosyntacticTag.contains('f')) {
                        feminine = false;
                    }
                    if (!info.morphosyntacticTag.contains('m')) {
                        masculine = false;
                    }
                }
                if (masculine && !feminine) {
                    colors.fgColor = QColor(127, 127, 255);
                } else if (!masculine && feminine) {
                    colors.fgColor = QColor(255, 127, 127);
                } else {
                    colors.fgColor = Qt::white;
                }
            }
        }
        out.charColors.push_back(colors);
    }

    return out;
}

QString FrenchProcessor::cleanWord(const QString word) {
    // equivalent to python clean_word

    QString split = word.normalized(QString::NormalizationForm_D);
    QString lower = split.toLower().replace("œ", "oe").replace("Œ", "oe").replace("æ", "ae").replace("Æ", "ae");
    return lower.normalized(QString::NormalizationForm_C);
}
