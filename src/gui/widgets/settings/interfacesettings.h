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

#ifndef INTERFACESETTINGS_H
#define INTERFACESETTINGS_H

#include <QWidget>

class QToolButton;

namespace Ui
{
    class InterfaceSettings;
}

class InterfaceSettings : public QWidget
{
    Q_OBJECT

public:
    InterfaceSettings(QWidget *parent = nullptr);
    ~InterfaceSettings();

protected:
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    void restoreDefaults();
    void restoreSaved();
    void applyChanges();

private:
    Ui::InterfaceSettings *m_ui;

    QColor m_subColor;
    QColor m_bgColor;
    QColor m_strokeColor;

    void setButtonColor(QToolButton *button, const QColor &color);
};

#endif // INTERFACESETTINGS_H