/***************************************************************************
 *   Copyright (C) 2015 The Qt Company Ltd.                                *
 *   Copyright (C) 2016-2023 Ilya Kotov, forkotov02@ya.ru                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <qpa/qplatformthemeplugin.h>
#include "qt6gtk2theme.h"

QT_BEGIN_NAMESPACE

class Qt6Gtk2ThemePlugin : public QPlatformThemePlugin
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "qt6gtk2.json")

public:
    QPlatformTheme *create(const QString &key, const QStringList &params) override;
};

QPlatformTheme *Qt6Gtk2ThemePlugin::create(const QString &key, const QStringList &params)
{
    Q_UNUSED(params);
    if (key.toLower() == QLatin1String("gtk2") || key.toLower() == QLatin1String("qt6gtk2") || key.toLower() == QLatin1String("qt5gtk2"))
        return new Qt6Gtk2Theme;

    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
