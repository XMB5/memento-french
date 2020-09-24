#include "playercontrols.h"
#include "ui_playercontrols.h"

PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent), m_ui(new Ui::PlayerControls)
{
    m_ui->setupUi(this);
    
    connect(m_ui->m_sliderProgress, &QSlider::sliderMoved, this, &PlayerControls::sliderMoved);
    connect(m_ui->m_buttonPlay, &QToolButton::clicked, this, &PlayerControls::pauseResume);
    connect(m_ui->m_buttonSeekForward, &QToolButton::clicked, this, &PlayerControls::seekForward);
    connect(m_ui->m_buttonSeekBackward, &QToolButton::clicked, this, &PlayerControls::seekBackward);
}

void PlayerControls::setDuration(int value)
{
    m_ui->m_sliderProgress->setRange(0, value);
    m_ui->m_labelTotal->setText(formatTime(value));
}

void PlayerControls::setPosition(int value)
{
    m_ui->m_sliderProgress->setValue(value);
    m_ui->m_labelCurrent->setText(formatTime(value));
}

void PlayerControls::setPaused(bool paused) {
    m_paused = paused;
    if (m_paused) {
        m_ui->m_buttonPlay->setIcon(QIcon::fromTheme(QString::fromUtf8(PLAY_ICON)));
    } else {
        m_ui->m_buttonPlay->setIcon(QIcon::fromTheme(QString::fromUtf8(PAUSE_ICON)));
    }
}

void PlayerControls::pauseResume()
{
    if (m_paused) {
        Q_EMIT play();
    } else {
        Q_EMIT pause();
    }
}

QString PlayerControls::formatTime(int time)
{
    int hours = time / SECONDS_IN_HOUR;
    int minutes = (time % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    int seconds = time % SECONDS_IN_MINUTE;
    
    QString formatted = QString("%1:%2").arg(minutes, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR))
                                        .arg(seconds, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR));
    if (hours) {
        formatted.prepend(QString("%1:").arg(hours));
    }

    return formatted;
}

PlayerControls::~PlayerControls()
{
    delete m_ui;
}