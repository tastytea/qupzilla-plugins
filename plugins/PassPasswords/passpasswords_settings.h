/* ============================================================
* PassPasswords - pass support plugin for QupZilla
* Copyright (C) 2017  tastytea <tastytea@tastytea.de>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#ifndef PASSPASSWORDS_SETTINGS
#define PASSPASSWORDS_SETTINGS

#include "passpasswords_settings.h"

#include <QDialog>

namespace Ui
{
class PassPasswordsSettings;
}

class PassPasswordBackend;

class PassPasswordsSettings : public QDialog
{
    Q_OBJECT

public:
    explicit PassPasswordsSettings(PassPasswordBackend* backend, QWidget* parent = 0);
    ~PassPasswordsSettings();

private slots:
    void saveSettings();

private:
    Ui::PassPasswordsSettings* ui;

    PassPasswordBackend* m_backend;
};

#endif // PASSPASSWORDS_SETTINGS
