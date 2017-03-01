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
#ifndef PASSPASSWORDBACKEND_H
#define PASSPASSWORDBACKEND_H

#include <QVector>
#include <QString>

#include "passwordbackends/passwordbackend.h"
#include "passwordmanager.h"

class PassPasswordBackend : public PasswordBackend
{
public:
    explicit PassPasswordBackend(const QString &settingsPath);

    QString name() const;

    QVector<PasswordEntry> getEntries(const QUrl &url);
    QVector<PasswordEntry> getAllEntries();

    void addEntry(const PasswordEntry &entry);
    void addEntry(const PasswordEntry &entry, const bool update);
    bool updateEntry(const PasswordEntry &entry);
    void updateLastUsed(PasswordEntry &entry);

    void removeEntry(const PasswordEntry &entry);
    void removeAll();

    void set_passdir(const QString &passdir);
    void set_rootnode(const QString &node);
    void set_writenode(const QString &node);

    const QString get_passdir() const;
    const QString get_rootnode() const;
    const QString get_writenode() const;

private:
    QString m_settingsPath;
    QString m_passdir;
    QString m_rootnode;
    QString m_writenode;
};

#endif // PASSPASSWORDBACKEND_H
