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
#include "passpasswords_settings.h"
#include "ui_passpasswords_settings.h"
#include "passpasswordbackend.h"

PassPasswordsSettings::PassPasswordsSettings(PassPasswordBackend* backend, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PassPasswordsSettings)
    , m_backend(backend)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    ui->text_passdir->setText(m_backend->get_passdir());
    ui->text_rootnode->setText(m_backend->get_rootnode());
    ui->text_writenode->setText(m_backend->get_writenode());

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void PassPasswordsSettings::saveSettings()
{
    m_backend->set_passdir(ui->text_passdir->text());
    m_backend->set_rootnode(ui->text_rootnode->text());
    m_backend->set_writenode(ui->text_writenode->text());

    close();
}

PassPasswordsSettings::~PassPasswordsSettings()
{
    delete ui;
}
