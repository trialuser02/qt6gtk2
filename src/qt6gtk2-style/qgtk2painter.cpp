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

#include "qgtk2painter_p.h"

#include <QtGlobal>
#if !defined(QT_NO_STYLE_GTK)

// This class is primarily a wrapper around the gtk painter functions
// and takes care of converting all such calls into cached Qt pixmaps.

#include "qgtkstyle_p_p.h"
#include <private/qhexstring_p.h>
#include <QWidget>
#include <QPixmapCache>

QT_BEGIN_NAMESPACE

// To recover alpha we apply the gtk painting function two times to
// white, and black window backgrounds. This can be used to
// recover the premultiplied alpha channel
QPixmap QGtk2Painter::renderTheme(uchar *bdata, uchar *wdata, const QRect &rect) const
{
    const int bytecount = rect.width() * rect.height() * 4;
    for (int index = 0; index < bytecount ; index += 4) {
        uchar val = bdata[index + GTK_BLUE];
        if (m_alpha) {
            int alphaval = qMax(bdata[index + GTK_BLUE] - wdata[index + GTK_BLUE],
                                bdata[index + GTK_GREEN] - wdata[index + GTK_GREEN]);
            alphaval = qMax(alphaval, bdata[index + GTK_RED] - wdata[index + GTK_RED]) + 255;
            bdata[index + QT_ALPHA] = alphaval;
        }
        bdata[index + QT_RED] = bdata[index + GTK_RED];
        bdata[index + QT_GREEN] = bdata[index + GTK_GREEN];
        bdata[index + QT_BLUE] = val;
    }
    QImage converted((const uchar*)bdata, rect.width(), rect.height(), m_alpha ?
                     QImage::Format_ARGB32_Premultiplied : QImage::Format_RGB32);

    if (m_hflipped || m_vflipped) {
        return QPixmap::fromImage(converted.mirrored(m_hflipped, m_vflipped));
    }
    // on raster graphicssystem we need to do a copy here, because
    // we intend to deallocate the qimage bits shortly after...
    return QPixmap::fromImage(converted.copy());
}

// This macro is responsible for painting any GtkStyle painting function onto a QPixmap
#define DRAW_TO_CACHE(draw_func)                                                                    \
    if (rect.width() > QWIDGETSIZE_MAX || rect.height() > QWIDGETSIZE_MAX)                          \
        return;                                                                                     \
    QRect pixmapRect(0, 0, rect.width(), rect.height());                                            \
    {                                                                                               \
        GdkPixmap *pixmap = gdk_pixmap_new((GdkDrawable*)(m_window->window),   \
                                                                rect.width(), rect.height(), -1);   \
        if (!pixmap)                                                                                \
            return;                                                                                 \
        style = gtk_style_attach (style, m_window->window);                       \
        gdk_draw_rectangle(pixmap, m_alpha ? style->black_gc : *style->bg_gc,  \
                                                true, 0, 0, rect.width(), rect.height());           \
        draw_func;                                                                                  \
        GdkPixbuf *imgb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8,             \
                                                           rect.width(), rect.height());            \
        if (!imgb)                                                                                  \
            return;                                                                                 \
        imgb = gdk_pixbuf_get_from_drawable(imgb, pixmap, nullptr, 0, 0, 0, 0,    \
                                                                 rect.width(), rect.height());      \
        uchar* bdata = (uchar*)gdk_pixbuf_get_pixels(imgb);                       \
        if (m_alpha) {                                                                              \
            gdk_draw_rectangle(pixmap, style->white_gc, true, 0, 0,            \
                                                    rect.width(), rect.height());                   \
            draw_func;                                                                              \
            GdkPixbuf *imgw = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8,         \
                                                               rect.width(), rect.height());        \
            if (!imgw)                                                                              \
                return;                                                                             \
            imgw = gdk_pixbuf_get_from_drawable(imgw, pixmap, nullptr, 0, 0, 0, 0,\
                                                                     rect.width(), rect.height());  \
            uchar* wdata = (uchar*)gdk_pixbuf_get_pixels(imgw);                   \
            cache = renderTheme(bdata, wdata, rect);                                                \
            g_object_unref(imgw);                                               \
        } else {                                                                                    \
            cache = renderTheme(bdata, nullptr, rect);                                                    \
        }                                                                                           \
        gdk_drawable_unref(pixmap);                                            \
        g_object_unref(imgb);                                                   \
    }

QGtk2Painter::QGtk2Painter() : QGtkPainter(), m_window(QGtkStylePrivate::gtkWidget("GtkWindow"))
{}

// Note currently painted without alpha for performance reasons
void QGtk2Painter::paintBoxGap(GtkWidget *gtkWidget, const gchar* part,
                              const QRect &paintRect, GtkStateType state,
                              GtkShadowType shadow, GtkPositionType gap_side,
                              gint x, gint width,
                              GtkStyle *style)
{
    if (!paintRect.isValid())
        return;

    QPixmap cache;
    QRect rect = paintRect;

    // To avoid exhausting cache on large tabframes we cheat a bit by
    // tiling the center part.

    const int maxHeight = 256;
    const int border = 16;
    if (rect.height() > maxHeight && (gap_side == GTK_POS_TOP || gap_side == GTK_POS_BOTTOM))
        rect.setHeight(2 * border + 1);

    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size(), gtkWidget)
                         % HexString<uchar>(gap_side)
                         % HexString<gint>(width)
                         % HexString<gint>(x);

    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_box_gap (style,
                                           pixmap,
                                           state,
                                           shadow,
                                           nullptr,
                                           gtkWidget,
                                           (const gchar*)part,
                                           0, 0,
                                           rect.width(),
                                           rect.height(),
                                           gap_side,
                                           x,
                                           width));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }
    if (rect.size() != paintRect.size()) {
        // We assume we can stretch the middle tab part
        // Note: the side effect of this is that pinstripe patterns will get fuzzy
        const QSize size = cache.size();
        // top part
        m_painter->drawPixmap(QRect(paintRect.left(), paintRect.top(),
                                    paintRect.width(), border), cache,
                             QRect(0, 0, size.width(), border));

        // tiled center part
        QPixmap tilePart(cache.width(), 1);
        QPainter scanLinePainter(&tilePart);
        scanLinePainter.drawPixmap(QRect(0, 0, tilePart.width(), tilePart.height()), cache, QRect(0, border, size.width(), 1));
        scanLinePainter.end();
        m_painter->drawTiledPixmap(QRect(paintRect.left(), paintRect.top() + border,
                                         paintRect.width(), paintRect.height() - 2*border), tilePart);

        // bottom part
        m_painter->drawPixmap(QRect(paintRect.left(), paintRect.top() + paintRect.height() - border,
                                    paintRect.width(), border), cache,
                             QRect(0, size.height() - border, size.width(), border));
    } else
        m_painter->drawPixmap(paintRect.topLeft(), cache);
}

void QGtk2Painter::paintBox(GtkWidget *gtkWidget, const gchar* part,
                           const QRect &paintRect, GtkStateType state,
                           GtkShadowType shadow, GtkStyle *style,
                           const QString &pmKey)
{
    if (!paintRect.isValid())
        return;

    QPixmap cache;
    QRect rect = paintRect;

    // To avoid exhausting cache on large tabframes we cheat a bit by
    // tiling the center part.

    const int maxHeight = 256;
    const int maxArea = 256*512;
    const int border = 32;
    if (rect.height() > maxHeight && (rect.width()*rect.height() > maxArea))
        rect.setHeight(2 * border + 1);

    QString pixmapName = uniqueName(QLS(part), state, shadow,
                                    rect.size(), gtkWidget) % pmKey;

    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_box (style,
                                           pixmap,
                                           state,
                                           shadow,
                                           nullptr,
                                           gtkWidget,
                                           part,
                                           0, 0,
                                           rect.width(),
                                           rect.height()));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }
    if (rect.size() != paintRect.size()) {
        // We assume we can stretch the middle tab part
        // Note: the side effect of this is that pinstripe patterns will get fuzzy
        const QSize size = cache.size();
        // top part
        m_painter->drawPixmap(QRect(paintRect.left(), paintRect.top(),
                                    paintRect.width(), border), cache,
                              QRect(0, 0, size.width(), border));

        // tiled center part
        QPixmap tilePart(cache.width(), 1);
        QPainter scanLinePainter(&tilePart);
        scanLinePainter.drawPixmap(QRect(0, 0, tilePart.width(), tilePart.height()), cache, QRect(0, border, size.width(), 1));
        scanLinePainter.end();
        m_painter->drawTiledPixmap(QRect(paintRect.left(), paintRect.top() + border,
                                         paintRect.width(), paintRect.height() - 2*border), tilePart);

        // bottom part
        m_painter->drawPixmap(QRect(paintRect.left(), paintRect.top() + paintRect.height() - border,
                                    paintRect.width(), border), cache,
                              QRect(0, size.height() - border, size.width(), border));
    } else
        m_painter->drawPixmap(paintRect.topLeft(), cache);
}

void QGtk2Painter::paintHline(GtkWidget *gtkWidget, const gchar* part,
                             const QRect &rect, GtkStateType state,
                             GtkStyle *style, int x1, int x2, int y,
                             const QString &pmKey)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, GTK_SHADOW_NONE, rect.size(), gtkWidget)
                         % HexString<int>(x1)
                         % HexString<int>(x2)
                         % HexString<int>(y)
                         % pmKey;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_hline (style,
                                         pixmap,
                                         state,
                                         nullptr,
                                         gtkWidget,
                                         part,
                                         x1, x2, y));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}

void QGtk2Painter::paintVline(GtkWidget *gtkWidget, const gchar* part,
                             const QRect &rect, GtkStateType state,
                             GtkStyle *style, int y1, int y2, int x,
                             const QString &pmKey)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, GTK_SHADOW_NONE, rect.size(), gtkWidget)
                        % HexString<int>(y1)
                        % HexString<int>(y2)
                        % HexString<int>(x)
                        % pmKey;

    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_vline (style,
                                         pixmap,
                                         state,
                                         nullptr,
                                         gtkWidget,
                                         part,
                                         y1, y2,
                                         x));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }
    m_painter->drawPixmap(rect.topLeft(), cache);
}


void QGtk2Painter::paintExpander(GtkWidget *gtkWidget,
                                const gchar* part, const QRect &rect,
                                GtkStateType state, GtkExpanderStyle expander_state,
                                GtkStyle *style, const QString &pmKey)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, GTK_SHADOW_NONE, rect.size(), gtkWidget)
                         % HexString<uchar>(expander_state)
                         % pmKey;

    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_expander (style, pixmap,
                                            state, nullptr,
                                            gtkWidget, part,
                                            rect.width()/2,
                                            rect.height()/2,
                                            expander_state));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}

void QGtk2Painter::paintFocus(GtkWidget *gtkWidget, const gchar* part,
                             const QRect &rect, GtkStateType state,
                             GtkStyle *style, const QString &pmKey)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, GTK_SHADOW_NONE, rect.size(), gtkWidget) % pmKey;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_focus (style, pixmap, state, nullptr,
                                         gtkWidget,
                                         part,
                                         0, 0,
                                         rect.width(),
                                         rect.height()));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}


void QGtk2Painter::paintResizeGrip(GtkWidget *gtkWidget, const gchar* part,
                                  const QRect &rect, GtkStateType state,
                                  GtkShadowType shadow, GdkWindowEdge edge,
                                  GtkStyle *style, const QString &pmKey)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size(), gtkWidget) % pmKey;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_resize_grip (style, pixmap, state,
                                               nullptr, gtkWidget,
                                               part, edge, 0, 0,
                                               rect.width(),
                                               rect.height()));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}


void QGtk2Painter::paintArrow(GtkWidget *gtkWidget, const gchar* part,
                             const QRect &arrowrect, GtkArrowType arrow_type,
                             GtkStateType state, GtkShadowType shadow,
                             gboolean fill, GtkStyle *style, const QString &pmKey)
{
    QRect rect = m_cliprect.isValid() ? m_cliprect : arrowrect;
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size())
                         % HexString<uchar>(arrow_type)
                         % pmKey;

    GdkRectangle gtkCliprect = {0, 0, rect.width(), rect.height()};
    int xOffset = m_cliprect.isValid() ? arrowrect.x() - m_cliprect.x() : 0;
    int yOffset = m_cliprect.isValid() ? arrowrect.y() - m_cliprect.y() : 0;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_arrow (style, pixmap, state, shadow,
                                         &gtkCliprect,
                                         gtkWidget,
                                         part,
                                         arrow_type, fill,
                                         xOffset, yOffset,
                                         arrowrect.width(),
                                         arrowrect.height()))
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}


void QGtk2Painter::paintHandle(GtkWidget *gtkWidget, const gchar* part, const QRect &rect,
                              GtkStateType state, GtkShadowType shadow,
                              GtkOrientation orientation, GtkStyle *style)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size())
                         % HexString<uchar>(orientation);

    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_handle (style,
                                          pixmap,
                                          state,
                                          shadow,
                                          nullptr,
                                          gtkWidget,
                                          part, 0, 0,
                                          rect.width(),
                                          rect.height(),
                                          orientation));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }
    m_painter->drawPixmap(rect.topLeft(), cache);
}


void QGtk2Painter::paintSlider(GtkWidget *gtkWidget, const gchar* part, const QRect &rect,
                              GtkStateType state, GtkShadowType shadow,
                              GtkStyle *style, GtkOrientation orientation,
                              const QString &pmKey)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size(), gtkWidget) % pmKey;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_slider (style,
                                          pixmap,
                                          state,
                                          shadow,
                                          nullptr,
                                          gtkWidget,
                                          part,
                                          0, 0,
                                          rect.width(),
                                          rect.height(),
                                          orientation));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }
    m_painter->drawPixmap(rect.topLeft(), cache);
}


void QGtk2Painter::paintShadow(GtkWidget *gtkWidget, const gchar* part,
                              const QRect &rect, GtkStateType state,
                              GtkShadowType shadow, GtkStyle *style,
                              const QString &pmKey)

{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size()) % pmKey;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_shadow(style, pixmap, state, shadow, nullptr,
                                         gtkWidget, part, 0, 0, rect.width(), rect.height()));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }
    m_painter->drawPixmap(rect.topLeft(), cache);
}

void QGtk2Painter::paintFlatBox(GtkWidget *gtkWidget, const gchar* part,
                               const QRect &rect, GtkStateType state,
                               GtkShadowType shadow, GtkStyle *style,
                               const QString &pmKey)
{
    if (!rect.isValid())
        return;
    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size()) % pmKey;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_flat_box (style,
                                            pixmap,
                                            state,
                                            shadow,
                                            nullptr,
                                            gtkWidget,
                                            part, 0, 0,
                                            rect.width(),
                                            rect.height()));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }
    m_painter->drawPixmap(rect.topLeft(), cache);
}

void QGtk2Painter::paintExtention(GtkWidget *gtkWidget,
                                 const gchar *part, const QRect &rect,
                                 GtkStateType state, GtkShadowType shadow,
                                 GtkPositionType gap_pos, GtkStyle *style)
{
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(QLS(part), state, shadow, rect.size(), gtkWidget)
                         % HexString<uchar>(gap_pos);

    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_extension (style, pixmap, state, shadow,
                                             nullptr, gtkWidget,
                                             (const gchar*)part, 0, 0,
                                             rect.width(),
                                             rect.height(),
                                             gap_pos));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}

void QGtk2Painter::paintOption(GtkWidget *gtkWidget, const QRect &radiorect,
                              GtkStateType state, GtkShadowType shadow,
                              GtkStyle *style, const QString &detail)

{
    QRect rect = m_cliprect.isValid() ? m_cliprect : radiorect;
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(detail, state, shadow, rect.size());
    GdkRectangle gtkCliprect = {0, 0, rect.width(), rect.height()};
    int xOffset = m_cliprect.isValid() ? radiorect.x() - m_cliprect.x() : 0;
    int yOffset = m_cliprect.isValid() ? radiorect.y() - m_cliprect.y() : 0;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_option(style, pixmap,
                                         state, shadow,
                                         &gtkCliprect,
                                         gtkWidget,
                                         detail.toLatin1().constData(),
                                         xOffset, yOffset,
                                         radiorect.width(),
                                         radiorect.height()));

        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}

void QGtk2Painter::paintCheckbox(GtkWidget *gtkWidget, const QRect &checkrect,
                                GtkStateType state, GtkShadowType shadow,
                                GtkStyle *style, const QString &detail)

{
    QRect rect = m_cliprect.isValid() ? m_cliprect : checkrect;
    if (!rect.isValid())
        return;

    QPixmap cache;
    QString pixmapName = uniqueName(detail, state, shadow, rect.size());
    GdkRectangle gtkCliprect = {0, 0, rect.width(), rect.height()};
    int xOffset = m_cliprect.isValid() ? checkrect.x() - m_cliprect.x() : 0;
    int yOffset = m_cliprect.isValid() ? checkrect.y() - m_cliprect.y() : 0;
    if (!m_usePixmapCache || !QPixmapCache::find(pixmapName, &cache)) {
        DRAW_TO_CACHE(gtk_paint_check (style,
                                         pixmap,
                                         state,
                                         shadow,
                                         &gtkCliprect,
                                         gtkWidget,
                                         detail.toLatin1().constData(),
                                         xOffset, yOffset,
                                         checkrect.width(),
                                         checkrect.height()));
        if (m_usePixmapCache)
            QPixmapCache::insert(pixmapName, cache);
    }

    m_painter->drawPixmap(rect.topLeft(), cache);
}

QT_END_NAMESPACE

#endif //!defined(QT_NO_STYLE_GTK)
