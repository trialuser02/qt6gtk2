/***************************************************************************
 *   Copyright (C) 2015 The Qt Company Ltd.                                *
 *   Copyright (C) 2016-2022 Ilya Kotov, forkotov02@ya.ru                  *
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

#ifndef QT6GTK2THEME_H
#define QT6GTK2THEME_H

#include <private/qgenericunixthemes_p.h>

QT_BEGIN_NAMESPACE

class Qt6Gtk2Theme : public QGnomeTheme
{
public:
    Qt6Gtk2Theme();

    virtual QVariant themeHint(ThemeHint hint) const override;
    virtual QString gtkFontName() const override;

    bool usePlatformNativeDialog(DialogType type) const override;
    QPlatformDialogHelper *createPlatformDialogHelper(DialogType type) const override;
};

QT_END_NAMESPACE

#endif // QT6GTK2THEME_H
