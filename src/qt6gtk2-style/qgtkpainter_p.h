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

#ifndef QGTKPAINTER_H
#define QGTKPAINTER_H

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

#include "qgtkglobal_p.h"
#include <QSize>
#include <QRect>
#include <QPoint>
#include <QPixmap>
#include <QPainter>

QT_BEGIN_NAMESPACE

class QGtkPainter
{
public:
    QGtkPainter();
    virtual ~QGtkPainter();

    void reset(QPainter *painter = nullptr);

    void setAlphaSupport(bool value) { m_alpha = value; }
    void setClipRect(const QRect &rect) { m_cliprect = rect; }
    void setFlipHorizontal(bool value) { m_hflipped = value; }
    void setFlipVertical(bool value) { m_vflipped = value; }
    void setUsePixmapCache(bool value) { m_usePixmapCache = value; }

    virtual void paintBoxGap(GtkWidget *gtkWidget, const gchar* part, const QRect &rect,
                             GtkStateType state, GtkShadowType shadow, GtkPositionType gap_side, gint x,
                             gint width, GtkStyle *style) = 0;
    virtual void paintBox(GtkWidget *gtkWidget, const gchar* part,
                          const QRect &rect, GtkStateType state, GtkShadowType shadow, GtkStyle *style,
                          const QString &pmKey = QString()) = 0;
    virtual void paintHline(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state, GtkStyle *style,
                            int x1, int x2, int y, const QString &pmKey = QString()) = 0;
    virtual void paintVline(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state, GtkStyle *style,
                            int y1, int y2, int x, const QString &pmKey = QString()) = 0;
    virtual void paintExpander(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state,
                               GtkExpanderStyle expander_state, GtkStyle *style, const QString &pmKey = QString()) = 0;
    virtual void paintFocus(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state, GtkStyle *style,
                            const QString &pmKey = QString()) = 0;
    virtual void paintResizeGrip(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state, GtkShadowType shadow,
                                 GdkWindowEdge edge, GtkStyle *style, const QString &pmKey = QString()) = 0;
    virtual void paintArrow(GtkWidget *gtkWidget, const gchar* part, const QRect &arrowrect, GtkArrowType arrow_type, GtkStateType state, GtkShadowType shadow,
                            gboolean fill, GtkStyle *style, const QString &pmKey = QString()) = 0;
    virtual void paintHandle(GtkWidget *gtkWidget, const gchar* part, const QRect &rect,
                             GtkStateType state, GtkShadowType shadow, GtkOrientation orientation, GtkStyle *style) = 0;
    virtual void paintSlider(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state, GtkShadowType shadow,
                             GtkStyle *style, GtkOrientation orientation, const QString &pmKey = QString()) = 0;
    virtual void paintShadow(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state, GtkShadowType shadow,
                             GtkStyle *style, const QString &pmKey = QString()) = 0;
    virtual void paintFlatBox(GtkWidget *gtkWidget, const gchar* part, const QRect &rect, GtkStateType state, GtkShadowType shadow, GtkStyle *style, const QString & = QString()) = 0;
    virtual void paintExtention(GtkWidget *gtkWidget, const gchar *part, const QRect &rect, GtkStateType state, GtkShadowType shadow,
                                GtkPositionType gap_pos, GtkStyle *style) = 0;
    virtual void paintOption(GtkWidget *gtkWidget, const QRect &rect, GtkStateType state, GtkShadowType shadow, GtkStyle *style, const QString &detail) = 0;
    virtual void paintCheckbox(GtkWidget *gtkWidget, const QRect &rect, GtkStateType state, GtkShadowType shadow, GtkStyle *style, const QString &detail) = 0;

protected:
    static QString uniqueName(const QString &key, GtkStateType state, GtkShadowType shadow, const QSize &size, GtkWidget *widget = nullptr);

    QPainter *m_painter;
    bool m_alpha;
    bool m_hflipped;
    bool m_vflipped;
    bool m_usePixmapCache;
    QRect m_cliprect;
};

QT_END_NAMESPACE

#endif //!defined(QT_NO_STYLE_QGTK)

#endif // QGTKPAINTER_H
