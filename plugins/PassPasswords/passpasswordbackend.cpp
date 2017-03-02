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
#include "passpasswordbackend.h"
#include "passplugin.h"

#include <QString>
#include <QStringList>
#include <QVector>
#include <QDirIterator>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QRegExp>

PassPasswordBackend::PassPasswordBackend(const QString &settingsPath)
    : PasswordBackend()
    , m_settingsPath(settingsPath)
    , m_passdir(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first()
                + "/.password-store")
{
    QSettings settings(m_settingsPath + QL1S("/extensions.ini"), QSettings::IniFormat);
    settings.beginGroup("PassPasswords");
    m_passdir = settings.value("passdir", m_passdir).toString();
    m_rootnode = settings.value("rootnode", "qupzilla").toString();
    m_writenode = settings.value("writenode", "qupzilla").toString();
    settings.endGroup();
}

QString PassPasswordBackend::name() const
{
    return PassPlugin::tr("pass");
}

QVector<PasswordEntry> PassPasswordBackend::getEntries(const QUrl &url)
{
    return getEntries(url, false);
}

QVector<PasswordEntry> PassPasswordBackend::getEntries(const QUrl &url, const bool &getall)
{
    QVector<PasswordEntry> list;
    const QString host = PasswordManager::createHost(url);
    QStringList filter;
    QVector<QStringList> records;

    if (!getall) {
        // Return nothing if an empty URL is supplied
        if (!url.isValid()) {
            return list;
        }

        filter << host + ".gpg" << host + "#*.gpg";
    }
    else {
        filter << "*.gpg";
    }

    QDirIterator dir(m_passdir + "/" + m_rootnode, filter,
                     QDir::Files, QDirIterator::Subdirectories);
    for (const QStringList &record : getRecords(dir)) {
        PasswordEntry data = extractData(record);

        if (data.data != "") {
            qDebug() << "PassPasswords: Found entry:" << data.host << data.id << data.data;
            list.append(data);
        }
    }

    if (list.empty()) {
        // If nothing is found, remove subdomain and try again
        if (url.topLevelDomain().count('.') > 0 &&
            host.count('.') > url.topLevelDomain().count('.')) {
            QString newhost = host;
            QUrl newurl = url;

            newhost.remove(0, host.indexOf('.') + 1);
            newurl.setHost(newhost);
            list = getEntries(newurl);
        }
    }

    return list;
}

QVector<PasswordEntry> PassPasswordBackend::getAllEntries()
{
    const QUrl url;

    return getEntries(url, true);
}

const QVector<QStringList> PassPasswordBackend::getRecords(QDirIterator &dir)
{
    QVector<QStringList> records;

    while (dir.hasNext()) {
        QProcess pass;
        QString output;
        QStringList record;
        // Build path for pass
        const QString relpath = dir.next().remove(QRegExp("^" + m_passdir + "/"))
                                          .remove(QRegExp(".gpg$"));
        QString basename = relpath.split('/').last();   // filename - .gpg

        pass.start("pass", QStringList() << "show" << relpath);
        pass.waitForFinished();
        output = pass.readAllStandardOutput();
        record = output.split('\n', QString::SkipEmptyParts);
        record << "host: " + basename.split('#').first();
        // I am using value() here because it returns "" if there is no # in basename
        record << "qupzilla-id: " + basename.split('#').value(1);
        records.append(record);
    }

    return records;
}

const PasswordEntry PassPasswordBackend::extractData(const QStringList &record)
{   // Extract data from record
    PasswordEntry data;
    QStringList::const_iterator it;

    data.password = record.at(0);

    for (it = ++record.constBegin(); it != record.constEnd(); ++it) {
        if ((*it).indexOf("username: ") == 0 ) {
            data.username = (*it).split(": ").value(1);
        }
        else if ((*it).indexOf("qupzilla: ") == 0 ) {
            data.data = (*it).split(": ").value(1).toLatin1();
        }
        else if ((*it).indexOf("host: ") == 0 ) {
            data.host = (*it).split(": ").value(1);
        }
        else if ((*it).indexOf("qupzilla-id: ") == 0 ) {
            data.id = (*it).split(": ").value(1).toInt();
        }
    }
    return data;
}

void PassPasswordBackend::addEntry(const PasswordEntry &entry)
{
    addEntry(entry, false);
}

void PassPasswordBackend::addEntry(const PasswordEntry &entry, const bool &update)
{
    QDir().mkpath(m_passdir + "/" + m_writenode);
    QProcess pass;
    const QString input = entry.password + '\n'
                        + "username: " + entry.username + '\n'
    // Basic Auth has no data, but we need non empty data to detect valid entries in getEntries()
                        + "qupzilla: " + ((entry.data != "") ? entry.data : "Basic Auth") + '\n';
    QString relpath = m_writenode + "/" + entry.host;
    QFileInfo file(m_passdir + "/" + relpath + ".gpg");

    if (!update) {
        unsigned int id = getEntries(QUrl(entry.host)).size();

        if (id > 0) {
            relpath = m_writenode + "/" + entry.host + "#" + QString::number(id);
            file.setFile(m_passdir + "/" + relpath + ".gpg");
        }
        while (file.exists()) {
            ++id;
            relpath = m_writenode + "/" + entry.host + "#" + QString::number(id);
            file.setFile(m_passdir + "/" + relpath + ".gpg");
        }
    }

    pass.start("pass", QStringList() << "insert" << "-m" << "-f" << relpath);
    pass.waitForStarted();
    pass.write(input.toLatin1());
    pass.closeWriteChannel();
    pass.waitForFinished();
}

bool PassPasswordBackend::updateEntry(const PasswordEntry &entry)
{
    addEntry(entry, true);
    return true;
}

void PassPasswordBackend::updateLastUsed(PasswordEntry &entry)
{
    qDebug() << "PassPasswords: updateLastUsed() NOT IMPLEMENTED" << entry.host;
}

void PassPasswordBackend::removeEntry(const PasswordEntry &entry)
{
    QProcess pass;
    QString relpath;
    QString id;
    QFileInfo file;

    if (entry.id != 0) {
        id = "#" + entry.id.toString();
    }
    file.setFile(m_passdir + "/" + m_writenode + "/" + entry.host + id + ".gpg");

    if (file.exists()) {
        relpath = m_writenode + "/" + entry.host;
    }
    else {
        // Entry is not under m_writenode, search under m_rootnode
        QDirIterator dir(m_passdir + "/" + m_rootnode,
                         QStringList() << "*" + entry.host + "*",
                         QDir::Files, QDirIterator::Subdirectories);
        if (dir.hasNext()) {
            relpath = dir.next().remove(QRegExp("^" + m_passdir + "/"))
                                .remove(QRegExp(".gpg$"));
        }
    }

    if (relpath != "") {
        qDebug() << "PassPlugin: deleting " + relpath;
        pass.start("pass", QStringList() << "rm" << "-f" << relpath);
        pass.waitForFinished();
    }
}

void PassPasswordBackend::removeAll()
{
    QProcess pass;

    pass.start("pass", QStringList() << "rm" << "-r" << "-f" << m_rootnode);
    pass.waitForFinished();
}

void PassPasswordBackend::set_passdir(const QString &passdir)
{
    QSettings settings(m_settingsPath + QL1S("/extensions.ini"), QSettings::IniFormat);
    settings.setValue("PassPasswords/passdir", passdir);
    m_passdir = passdir;
}

void PassPasswordBackend::set_rootnode(const QString &node)
{
    QSettings settings(m_settingsPath + QL1S("/extensions.ini"), QSettings::IniFormat);
    settings.setValue("PassPasswords/rootnode", node);
    m_rootnode = node;
}

void PassPasswordBackend::set_writenode(const QString &node)
{
    QSettings settings(m_settingsPath + QL1S("/extensions.ini"), QSettings::IniFormat);
    settings.setValue("PassPasswords/writenode", node);
    m_writenode = node;
}

const QString PassPasswordBackend::get_passdir() const
{
    return m_passdir;
}

const QString PassPasswordBackend::get_rootnode() const
{
    return m_rootnode;
}

const QString PassPasswordBackend::get_writenode() const
{
    return m_writenode;
}
