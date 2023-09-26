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

#include "qgtkpainter_p.h"

#if !defined(QT_NO_STYLE_GTK)

#include <private/qhexstring_p.h>

QT_BEGIN_NAMESPACE

QGtkPainter::QGtkPainter()
{
    reset(nullptr);
}

QGtkPainter::~QGtkPainter()
{
}

void QGtkPainter::reset(QPainter *painter)
{
    m_painter = painter;
    m_alpha = true;
    m_hflipped = false;
    m_vflipped = false;
    m_usePixmapCache = true;
    m_cliprect = QRect();
}

QString QGtkPainter::uniqueName(const QString &key, GtkStateType state, GtkShadowType shadow,
                                const QSize &size, GtkWidget *widget)
{
    // Note the widget arg should ideally use the widget path, though would compromise performance
    QString tmp = key
                  % HexString<uint>(state)
                  % HexString<uint>(shadow)
                  % HexString<uint>(size.width())
                  % HexString<uint>(size.height())
                  % HexString<quint64>(quint64(widget));
    return tmp;
}

QT_END_NAMESPACE

#endif //!defined(QT_NO_STYLE_GTK)
