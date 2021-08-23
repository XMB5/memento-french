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

#include "audiosourcesettings.h"
#include "ui_audiosourcesettings.h"

#include "../../../util/constants.h"
#include "../../../util/iconfactory.h"

#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

#define COL_NAME    0
#define COL_URL     1
#define COL_MD5     2

AudioSourceSettings::AudioSourceSettings(QWidget *parent) 
    : QWidget(parent), m_ui(new Ui::AudioSourceSettings)
{
    m_ui->setupUi(this);

    m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    IconFactory *factory = IconFactory::create();
    m_ui->buttonUp->setIcon(factory->getIcon(IconFactory::Icon::up));
    m_ui->buttonDown->setIcon(factory->getIcon(IconFactory::Icon::down));

    /* Table Actions */
    connect(m_ui->table, &QTableWidget::cellChanged,        this, &AudioSourceSettings::updateRows);
    connect(m_ui->table, &QTableWidget::currentCellChanged, this, &AudioSourceSettings::updateButtons);
    connect(m_ui->table, &QTableWidget::cellChanged,        this, &AudioSourceSettings::updateButtons);

    /* Up/Down Buttons */
    connect(m_ui->buttonUp,   &QToolButton::clicked, this, &AudioSourceSettings::moveUp);
    connect(m_ui->buttonDown, &QToolButton::clicked, this, &AudioSourceSettings::moveDown);

    /* Dialog Buttons */
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply),
        &QPushButton::clicked, this, &AudioSourceSettings::applyChanges
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::RestoreDefaults),
        &QPushButton::clicked, this, &AudioSourceSettings::restoreDefaults
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset),
        &QPushButton::clicked, this, &AudioSourceSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Help), 
        &QPushButton::clicked, this, &AudioSourceSettings::showHelp
    );

    /* Make sure at least the default are saved to settings */
    restoreSaved();
    applyChanges();
}

AudioSourceSettings::~AudioSourceSettings()
{
    delete m_ui;
}

void AudioSourceSettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    restoreSaved();
    m_ui->buttonDown->setEnabled(false);
    m_ui->buttonUp->setEnabled(false);
}

void AudioSourceSettings::applyChanges()
{
    QString error = verifyNames();
    if (!error.isEmpty())
    {
        QMessageBox::critical(this, "Audio Source Error", 
            "Could not apply changes:\n" + error
        );
        return;
    }

    QSettings settings;
    settings.remove(SETTINGS_AUDIO_SRC);
    settings.beginWriteArray(SETTINGS_AUDIO_SRC);

    QTableWidget *table = m_ui->table;
    for (size_t i = 0; i < table->rowCount() - 1; ++i)
    {
        settings.setArrayIndex(i);

        QString name = itemEmpty(i, COL_NAME) ? "" : table->item(i, COL_NAME)->text();
        QString url  = itemEmpty(i, COL_URL)  ? "" : table->item(i, COL_URL)->text();
        QString md5  = itemEmpty(i, COL_MD5)  ? "" : table->item(i, COL_MD5)->text();

        settings.setValue(SETTINGS_AUDIO_SRC_NAME, name);
        settings.setValue(SETTINGS_AUDIO_SRC_URL,  url);
        settings.setValue(SETTINGS_AUDIO_SRC_MD5,  md5);
    }
    settings.endArray();

    if (table->rowCount() == 1)
    {
        restoreSaved();
    }
}

void AudioSourceSettings::restoreDefaults()
{
    QTableWidget *table = m_ui->table;
    table->clearContents();
    table->setRowCount(2);

    table->setItem(0, COL_NAME, new QTableWidgetItem(SETTINGS_AUDIO_SRC_NAME_DEFAULT));
    table->setItem(0, COL_URL,  new QTableWidgetItem(SETTINGS_AUDIO_SRC_URL_DEFAULT));
    table->setItem(0, COL_MD5,  new QTableWidgetItem(SETTINGS_AUDIO_SRC_MD5_DEFAULT));
}

void AudioSourceSettings::restoreSaved()
{
    QSettings settings;
    size_t size = settings.beginReadArray(SETTINGS_AUDIO_SRC);
    if (size == 0)
    {
        restoreDefaults();
        return;
    }

    QTableWidget *table = m_ui->table;
    table->clearContents();
    table->setRowCount(size + 1);
    for (size_t i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);

        table->setItem(i, COL_NAME, 
            new QTableWidgetItem(settings.value(SETTINGS_AUDIO_SRC_NAME).toString())
        );
        table->setItem(i, COL_URL, 
            new QTableWidgetItem(settings.value(SETTINGS_AUDIO_SRC_URL).toString())
        );
        table->setItem(i, COL_MD5, 
            new QTableWidgetItem(settings.value(SETTINGS_AUDIO_SRC_MD5).toString())
        );
    }
    settings.endArray();
}

void AudioSourceSettings::showHelp()
{
    QMessageBox::information(this, "Audio Source Help", 
        "<b>Source Name</b>: The name of the audio source as it will appear in Memento."
        "<br><br>"
        "<b>URL</b>: The URL of the audio source. "
            "Supports inserting {expression} marker into the URL. "
            "See Anki Integration Help for more information."
        "<br><br>"
        "<b>MD5 Skip Hash</b>: Audio that matches this MD5 hash will be ignored."
    );
}

void AudioSourceSettings::updateButtons()
{
    const int currentRow = m_ui->table->currentRow();
    const int rowCount   = m_ui->table->rowCount();

    m_ui->buttonUp->setEnabled(0 < currentRow && currentRow < rowCount - 1);
    m_ui->buttonDown->setEnabled(currentRow < rowCount - 2);
}

void AudioSourceSettings::moveRow(const int row, const int step)
{
    QTableWidget *table = m_ui->table;

    const bool signalsBlocked = table->signalsBlocked();
    table->blockSignals(true);

    QTableWidgetItem *name_1 = table->takeItem(row, COL_NAME);
    QTableWidgetItem *url_1  = table->takeItem(row, COL_URL);
    QTableWidgetItem *md5_1  = table->takeItem(row, COL_MD5);

    const int newRow = row + step;
    QTableWidgetItem *name_2 = table->takeItem(newRow, COL_NAME);
    QTableWidgetItem *url_2  = table->takeItem(newRow, COL_URL);
    QTableWidgetItem *md5_2  = table->takeItem(newRow, COL_MD5);

    table->setItem(newRow, COL_NAME, name_1);
    table->setItem(newRow, COL_URL,  url_1);
    table->setItem(newRow, COL_MD5,  md5_1);

    table->setItem(row, COL_NAME, name_2);
    table->setItem(row, COL_URL,  url_2);
    table->setItem(row, COL_MD5,  md5_2);

    table->blockSignals(signalsBlocked);
}

void AudioSourceSettings::moveUp()
{
    moveRow(m_ui->table->currentRow(), -1);
    m_ui->table->selectRow(m_ui->table->currentRow() - 1);
}

void AudioSourceSettings::moveDown()
{
    moveRow(m_ui->table->currentRow(), 1);
    m_ui->table->selectRow(m_ui->table->currentRow() + 1);
}

QString AudioSourceSettings::verifyNames() const
{
    QSet<QString> names;
    QTableWidget *table = m_ui->table;
    for (size_t i = 0; i < table->rowCount() - 1; ++i)
    {
        if (itemEmpty(i, COL_NAME))
        {
            return QString("Missing name at row %1.").arg(i + 1);
        }

        QString name = table->item(i, COL_NAME)->text();
        if (names.contains(name))
        {
            return QString("Duplicate name at row %1.").arg(i + 1);
        }
        names.insert(name);
    }

    return "";
}

void AudioSourceSettings::updateRows()
{
    /* Remove empty rows */
    QTableWidget *table = m_ui->table;
    for (size_t i = 0; i < table->rowCount() - 1; ++i)
    {
        if (rowEmpty(i))
        {
            table->removeRow(i--);
        }
    }

    /* Add an empty row at the end if needed */
    size_t lastRow = table->rowCount() - 1;
    if (!rowEmpty(lastRow))
    {
        table->insertRow(lastRow + 1);
    }
}

bool inline AudioSourceSettings::itemEmpty(const int row, const int col) const
{
    QTableWidgetItem *item = m_ui->table->item(row, col);
    return item == nullptr || item->text().isEmpty();
}

bool inline AudioSourceSettings::rowEmpty(const int row) const
{
    for (size_t i = 0; i < m_ui->table->columnCount(); ++i)
    {
        if (!itemEmpty(row, i))
        {
            return false;
        }
    }
    return true;
}
