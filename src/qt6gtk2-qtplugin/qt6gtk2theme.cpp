/***************************************************************************
 *   Copyright (C) 2015 The Qt Company Ltd.                                *
 *   Copyright (C) 2016-2024 Ilya Kotov, forkotov02@ya.ru                  *
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

#include "qt6gtk2theme.h"
#include "qt6gtk2dialoghelpers.h"
#include <QVariant>

#undef signals
#include <gtk/gtk.h>

#include <X11/Xlib.h>

QT_BEGIN_NAMESPACE

static QVariant gtkSetting(const gchar *propertyName)
{
    GtkSettings *settings = gtk_settings_get_default();
    GValue value = G_VALUE_INIT;
    QVariant ret;

    g_object_get_property(G_OBJECT(settings), propertyName, &value);
    if (G_VALUE_HOLDS_INT(&value)) {
        ret = QVariant(g_value_get_int(&value));
    } else if (G_VALUE_HOLDS_UINT(&value)) {
        ret = QVariant(g_value_get_uint(&value));
    } else if (G_VALUE_HOLDS_FLOAT(&value)) {
        ret = QVariant(g_value_get_float(&value));
    } else if (G_VALUE_HOLDS_STRING(&value)) {
        ret = QVariant(QString::fromUtf8(g_value_get_string(&value)));
    } else if (G_VALUE_HOLDS_BOOLEAN(&value)) {
        ret = QVariant(g_value_get_boolean(&value));
    } else {
        ret = QVariant();
    }
    g_value_unset(&value);
    return ret;
}

Qt6Gtk2Theme::Qt6Gtk2Theme()
{
    // gtk_init will reset the Xlib error handler, and that causes
    // Qt applications to quit on X errors. Therefore, we need to manually restore it.
    int (*oldErrorHandler)(Display *, XErrorEvent *) = XSetErrorHandler(nullptr);

    gtk_init(nullptr, nullptr);

    XSetErrorHandler(oldErrorHandler);
}

QVariant Qt6Gtk2Theme::themeHint(QPlatformTheme::ThemeHint hint) const
{
    switch (hint) {
    case QPlatformTheme::CursorFlashTime:
        // As close to GTK as possible.
        if (gtkSetting("gtk-cursor-blink").toBool() && gtkSetting("gtk-cursor-blink-timeout").toInt() != 0) {
            return gtkSetting("gtk-cursor-blink-time");
        } else {
            return QVariant((int) 0);
        }
    case QPlatformTheme::PasswordMaskDelay:
        return gtkSetting("gtk-entry-password-hint-timeout");
    case QPlatformTheme::DialogButtonBoxButtonsHaveIcons:
        return gtkSetting("gtk-button-images");
    case QPlatformTheme::ShowShortcutsInContextMenus:
        return gtkSetting("gtk-enable-accels");
    case QPlatformTheme::SystemIconThemeName:
        return gtkSetting("gtk-icon-theme-name");
    case QPlatformTheme::SystemIconFallbackThemeName:
        return gtkSetting("gtk-fallback-icon-theme");
    case QPlatformTheme::StyleNames:
    {
        QStringList styleNames;
        styleNames << "qt6gtk2";
        //styleNames << QGnomeTheme::themeHint(hint).toStringList();
        return styleNames;
    }
    default:
        return QGnomeTheme::themeHint(hint);
    }
}

QString Qt6Gtk2Theme::gtkFontName() const
{
    QString cfgFontName = gtkSetting("gtk-font-name").toString();
    if (!cfgFontName.isEmpty())
        return cfgFontName;
    return QGnomeTheme::gtkFontName();
}

bool Qt6Gtk2Theme::usePlatformNativeDialog(DialogType type) const
{
    switch (type) {
    case ColorDialog:
        return true;
    case FileDialog:
        return true;
    case FontDialog:
        return true;
    default:
        return false;
    }
}

QPlatformDialogHelper *Qt6Gtk2Theme::createPlatformDialogHelper(DialogType type) const
{
    switch (type) {
    case ColorDialog:
        return new Qt6Gtk2ColorDialogHelper;
    case FileDialog:
        return new Qt6Gtk2FileDialogHelper;
    case FontDialog:
        return new Qt6Gtk2FontDialogHelper;
    default:
        return nullptr;
    }
}

QT_END_NAMESPACE
