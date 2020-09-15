/***************************************************************************
 *   Copyright (C) 2015 The Qt Company Ltd.                                *
 *   Copyright (C) 2016-2020 Ilya Kotov, forkotov02@ya.ru                  *
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

#ifndef QGTKSTYLE_P_P_H
#define QGTKSTYLE_P_P_H

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

#include <QString>
#include <QStringBuilder>
#include <QCoreApplication>
#include <QFileDialog>
#include <QCommonStyle>

#include <private/qcommonstyle_p.h>
#include "qgtkstyle_p.h"
#include "qgtkglobal_p.h"

#define Q_GTK_IS_WIDGET(widget) widget && G_TYPE_CHECK_INSTANCE_TYPE ((widget), gtk_widget_get_type())

QT_BEGIN_NAMESPACE

class QHashableLatin1Literal
{
public:
    int size() const { return m_size; }
    const char *data() const { return m_data; }

#ifdef __SUNPRO_CC
        QHashableLatin1Literal(const char* str)
        : m_size(strlen(str)), m_data(str) {}
#else
    template <int N>
        QHashableLatin1Literal(const char (&str)[N])
        : m_size(N - 1), m_data(str) {}
#endif

    QHashableLatin1Literal(const QHashableLatin1Literal &other)
        : m_size(other.m_size), m_data(other.m_data)
    {}

    QHashableLatin1Literal &operator=(const QHashableLatin1Literal &other)
    {
        if (this == &other)
            return *this;
        *const_cast<int *>(&m_size) = other.m_size;
        *const_cast<char **>(&m_data) = const_cast<char *>(other.m_data);
        return *this;
    }

    QString toString() const { return QString::fromLatin1(m_data, m_size); }

    static QHashableLatin1Literal fromData(const char *str)
    {
        return QHashableLatin1Literal(str, qstrlen(str));
    }

private:
    QHashableLatin1Literal(const char *str, int length)
        : m_size(length), m_data(str)
    {}

    const int m_size;
    const char *m_data;
};

bool operator==(const QHashableLatin1Literal &l1, const QHashableLatin1Literal &l2);
inline bool operator!=(const QHashableLatin1Literal &l1, const QHashableLatin1Literal &l2) { return !operator==(l1, l2); }
uint qHash(const QHashableLatin1Literal &key);

QT_END_NAMESPACE

typedef struct _XDisplay Display;

QT_BEGIN_NAMESPACE

class QGtkPainter;
class QGtkStylePrivate;

class QGtkStyleFilter : public QObject
{
public:
    QGtkStyleFilter(QGtkStylePrivate* sp)
        : stylePrivate(sp)
    {}
private:
    QGtkStylePrivate* stylePrivate;
    bool eventFilter(QObject *obj, QEvent *e) override;
};

class QGtkStylePrivate : public QCommonStylePrivate
{
    Q_DECLARE_PUBLIC(QGtkStyle)
public:
    QGtkStylePrivate();
    ~QGtkStylePrivate();

    QGtkStyleFilter filter;

    static QGtkPainter* gtkPainter(QPainter *painter = nullptr);
    static GtkWidget* gtkWidget(const QHashableLatin1Literal &path);
    static GtkStyle* gtkStyle(const QHashableLatin1Literal &path = QHashableLatin1Literal("GtkWindow"));
    static void gtkWidgetSetFocus(GtkWidget *widget, bool focus);

    virtual void initGtkMenu() const;
    virtual void initGtkTreeview() const;
    virtual void initGtkWidgets() const;

    static void cleanupGtkWidgets();

    static bool isKDE4Session();
    void applyCustomPaletteHash();
    static QFont getThemeFont();
    static bool isThemeAvailable() { return gtkStyle() != nullptr; }

    static QString getThemeName();
    virtual int getSpinboxArrowSize() const;

    virtual QPalette gtkWidgetPalette(const QHashableLatin1Literal &gtkWidgetName) const;

protected:
    typedef QHash<QHashableLatin1Literal, GtkWidget*> WidgetMap;

    static inline void destroyWidgetMap()
    {
        cleanupGtkWidgets();
        delete widgetMap;
        widgetMap = nullptr;
    }

    static inline WidgetMap *gtkWidgetMap()
    {
        if (!widgetMap) {
            widgetMap = new WidgetMap();
            qAddPostRoutine(destroyWidgetMap);
        }
        return widgetMap;
    }

    static QStringList extract_filter(const QString &rawFilter);

    virtual GtkWidget* getTextColorWidget() const;
    static void setupGtkWidget(GtkWidget* widget);
    static void addWidgetToMap(GtkWidget* widget);
    static void addAllSubWidgets(GtkWidget *widget, gpointer v = nullptr);
    static void addWidget(GtkWidget *widget);
    static void removeWidgetFromMap(const QHashableLatin1Literal &path);

    virtual void init();

    enum {
        menuItemFrame        =  2, // menu item frame width
        menuItemHMargin      =  3, // menu item hor text margin
        menuArrowHMargin     =  6, // menu arrow horizontal margin
        menuItemVMargin      =  2, // menu item ver text margin
        menuRightBorder      = 15, // right border on menus
        menuCheckMarkWidth   = 12  // checkmarks width on menus
    };

private:
    static QList<QGtkStylePrivate *> instances;
    static WidgetMap *widgetMap;
    friend class QGtkStyleUpdateScheduler;
};

// Helper to ensure that we have polished all our gtk widgets
// before updating our own palettes
class QGtkStyleUpdateScheduler : public QObject
{
    Q_OBJECT
public slots:
    void updateTheme();
};

QT_END_NAMESPACE

#endif // !QT_NO_STYLE_GTK
#endif // QGTKSTYLE_P_P_H
