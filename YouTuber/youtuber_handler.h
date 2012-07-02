/* ============================================================
* YouTuber plugin for QupZilla
* Copyright (C) 2012  David Rosca <nowrep@gmail.com>
* Copyright (C) 2012  Mladen Pejaković <pejakm@gmail.com>
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
#ifndef YOUTUBER_HANDLER_H
#define YOUTUBER_HANDLER_H

#include <QObject>
#include <QWebElement>
#include <QMessageBox>
#include <QProcess>
#include <QRegExp>
#include <QWebHitTestResult>
#include <QMenu>
#include <QWeakPointer>

class QMouseEvent;
class QWebElement;
class QLabel;
class WebView;
class YouTuber_Handler : public QObject
{
    Q_OBJECT
public:
    explicit YouTuber_Handler(const QString &sPath, QObject* parent = 0);

    bool handleMousePress(QObject* obj, QMouseEvent* event);

    QString settingsFile();
    void loadSettings();
    void populateWebViewMenu(QMenu* menu, WebView* view, const QWebHitTestResult &r);

signals:

public slots:

private slots:
    void startExternalHandler();

private:
    QWeakPointer<WebView> m_view;

    QString m_extExe;
    QString m_extArgs;
    QString m_settingsFile;
};

#endif // YOUTUBER_HANDLER_H