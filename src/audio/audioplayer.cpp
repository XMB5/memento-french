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

#include "audioplayer.h"

#include "../util/globalmediator.h"

#include <mpv/client.h>
#include <cstdlib>

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QThreadPool>
#include <QFileInfo>
#include <QProcess>

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{
    m_mpv = mpv_create();
    if (!m_mpv)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
            "Could not start mpv",
            "AudioPlayer: Could not create mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    mpv_set_option_string(m_mpv, "config",         "no");
    mpv_set_option_string(m_mpv, "terminal",       "no");
    mpv_set_option_string(m_mpv, "force-window",   "no");
    mpv_set_option_string(m_mpv, "input-terminal", "no");

    if (mpv_initialize(m_mpv) < 0)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
            "Could not start mpv",
            "AudioPlayer: Failed to initialize mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    QString tlds[] = {"fr", "ca"};
    for (auto &tld : tlds) {
        for (int slow = 0; slow <= 1; slow++) {
            this->audioSources.push_back(AudioSource{
                "fr-" + tld + (slow == 1 ? "-slow" : ""),
                "fr",
                tld,
                bool(slow)
            });
        }
    }
}

AudioPlayer::~AudioPlayer()
{
    mpv_terminate_destroy(m_mpv);
    clearFiles();
}

void AudioPlayer::clearFiles()
{
    m_fileLock.lock();
    for (QTemporaryFile *file : m_files)
    {
        delete file;
    }
    m_files.clear();
    m_fileLock.unlock();
}

bool AudioPlayer::playFile(const QTemporaryFile *file)
{
    if (file == nullptr)
        return false;

    QByteArray fileName = QFileInfo(*file).absoluteFilePath().toUtf8();
    const char *args[3] = {
        "loadfile",
        fileName,
        NULL
    };
    
    if (mpv_command(m_mpv, args) < 0)
    {
        return false;
    }

    return true;
}

AudioPlayerReply *AudioPlayer::playAudio(QString text, QString lang, QString tld, bool slow)
{
    /* Check if the file exists */
    m_fileLock.lock();
    QString cacheKey = text + '\x00' + lang + '\x00' + tld + '\x00' + (slow ? 's' : 'f');  // null bytes are allowed in QString
    QTemporaryFile *file = m_files[cacheKey];
    if (file)
    {
        playFile(file);
        m_fileLock.unlock();
        return nullptr;
    }
    m_fileLock.unlock();

    auto *audioReply = new AudioPlayerReply;

    // download audio
    file = new QTemporaryFile;
    // need to open temporary file to create it (otherwise fileName() will return empty string)
    if (!file->open()) {
        throw std::runtime_error("failed to open temporary file");
    }
    file->close();

    QProcess *process = new QProcess;

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](int statusCode, QProcess::ExitStatus exitStatus) {
        if (statusCode == 0 && exitStatus == QProcess::NormalExit) {
            // add to cache
            m_fileLock.lock();
            m_files[cacheKey] = file;

            bool res = this->playFile(file);
            m_fileLock.unlock();
            Q_EMIT audioReply->result(res);
        } else {
            qDebug() << "process failed with status code" << statusCode << "exit status" << exitStatus << "stderr" << process->readAllStandardError();
            Q_EMIT audioReply->result(false);
        }

        process->deleteLater();
        audioReply->deleteLater();
    });
    connect(process, &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            qDebug() << "process failed to start" << process->errorString();
            Q_EMIT audioReply->result(false);
            audioReply->deleteLater();
            process->deleteLater();
        }
    });

    QStringList args;
    args << "--lang" << lang << "--nocheck" << "--tld" << tld << "--output" << QFileInfo(*file).absoluteFilePath();
    if (slow) {
        args << "--slow";
    }
    args << "--" << text;

    qDebug() << "run gtts-cli" << args;
    process->start("gtts-cli", args);

    return audioReply;
}
