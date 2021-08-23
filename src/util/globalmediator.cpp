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

#include "globalmediator.h"

#include "directoryutils.h"

GlobalMediator::GlobalMediator(QObject *parent) : QObject(parent)
{
    m_dictionary   = nullptr;
    m_ankiClient   = nullptr;
    m_player       = nullptr;
    m_playerWidget = nullptr;
    m_subList      = nullptr;
    m_audioPlayer  = nullptr;
    m_frenchProcessor = nullptr;
}

GlobalMediator *GlobalMediator::createGlobalMedaitor()
{
    m_mediator = new GlobalMediator;
    return m_mediator;
}

GlobalMediator *GlobalMediator::getGlobalMediator()
{
    return m_mediator;
}

Dictionary *GlobalMediator::getDictionary() const
{
    return m_dictionary;
}

PlayerAdapter *GlobalMediator::getPlayerAdapter() const
{
    return m_player;
}

QWidget *GlobalMediator::getPlayerWidget() const
{
    return m_playerWidget;
}

AnkiClient *GlobalMediator::getAnkiClient() const
{
    return m_ankiClient;
}

SubtitleListWidget *GlobalMediator::getSubtitleListWidget() const
{
    return m_subList;
}

AudioPlayer *GlobalMediator::getAudioPlayer() const
{
    return m_audioPlayer;
}

FrenchProcessor *GlobalMediator::getFrenchProcessor() const {
    return m_frenchProcessor;
}

GlobalMediator *GlobalMediator::setDictionary(Dictionary *dictionary)
{
    m_dictionary = dictionary;
    return m_mediator;
}

GlobalMediator *GlobalMediator::setPlayerAdapter(PlayerAdapter *player)
{
    m_player = player;
    return m_mediator;
}

GlobalMediator *GlobalMediator::setPlayerWidget(QWidget *widget)
{
    m_playerWidget = widget;
    return m_mediator;
}

GlobalMediator *GlobalMediator::setAnkiClient(AnkiClient *client)
{
    m_ankiClient = client;
    return m_mediator;
}

GlobalMediator *GlobalMediator::setSubtitleList(SubtitleListWidget *subList)
{
    m_subList = subList;
    return m_mediator;
}

GlobalMediator *GlobalMediator::setAudioPlayer(AudioPlayer *audioPlayer)
{
    m_audioPlayer = audioPlayer;
    return m_mediator;
}

GlobalMediator *GlobalMediator::setFrenchProcessor(FrenchProcessor *frenchProcessor) {
    m_frenchProcessor = frenchProcessor;
    return m_mediator;
}