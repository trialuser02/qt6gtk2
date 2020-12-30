/***************************************************************************
 *   Copyright (C) 2015 The Qt Company Ltd.                                *
 *   Copyright (C) 2016-2021 Ilya Kotov, forkotov02@ya.ru                  *
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

#ifndef QGTKGLOBAL_P_H
#define QGTKGLOBAL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGlobal>
#if !defined(QT_NO_STYLE_GTK)

#undef signals // Collides with GTK symbols
#include <gtk/gtk.h>

typedef unsigned long XID;

#undef GTK_OBJECT_FLAGS
#define GTK_OBJECT_FLAGS(obj)(((GtkObject*)(obj))->flags)

#define QLS(x) QLatin1String(x)

QT_BEGIN_NAMESPACE

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#   define QT_RED 3
#   define QT_GREEN 2
#   define QT_BLUE 1
#   define QT_ALPHA 0
#else
#   define QT_RED 0
#   define QT_GREEN 1
#   define QT_BLUE 2
#   define QT_ALPHA 3
#endif
#   define GTK_RED 2
#   define GTK_GREEN 1
#   define GTK_BLUE 0
#   define GTK_ALPHA 3

QT_END_NAMESPACE

#endif // !QT_NO_STYLE_GTK
#endif // QGTKGLOBAL_P_H
