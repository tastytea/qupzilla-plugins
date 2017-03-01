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
#include "passplugin.h"
#include "passpasswordbackend.h"
#include "passpasswords_settings.h"
#include "pluginproxy.h"
#include "browserwindow.h"

#include <QTranslator>
#include <QPushButton>
#include <QLineEdit>

PassPlugin::PassPlugin()
    : QObject()
    , m_backend(0)
{
}

PluginSpec PassPlugin::pluginSpec()
{
    PluginSpec spec;
    spec.name = "Pass Passwords";
    spec.info = "pass password backend";
    spec.description = "Provides support for pass (http://www.passwordstore.org/)";
    spec.version = "0.4.5";
    spec.author = "tastytea <tastytea@tastytea.de>";
    spec.icon = QPixmap(":passpasswords/data/icon.png");
    spec.hasSettings = true;

    return spec;
}

void PassPlugin::init(InitState state, const QString &settingsPath)
{
    Q_UNUSED(state);

    m_backend = new PassPasswordBackend(settingsPath);
    QZ_REGISTER_PASSWORD_BACKEND("pass", m_backend);
}

void PassPlugin::unload()
{
    QZ_UNREGISTER_PASSWORD_BACKEND(m_backend);
    delete m_backend;
}

bool PassPlugin::testPlugin()
{
    // Require the version that the plugin was built with
    return (Qz::VERSION == QLatin1String(QUPZILLA_VERSION));
}

QTranslator* PassPlugin::getTranslator(const QString &locale)
{
    QTranslator* translator = new QTranslator(this);
    translator->load(locale, ":/passpasswords/locale/");
    return translator;
}

void PassPlugin::showSettings(QWidget* parent)
{
    PassPasswordsSettings* dialog = new PassPasswordsSettings(m_backend, parent);
    dialog->open();
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(PassPasswords, PassPlugin)
#endif
