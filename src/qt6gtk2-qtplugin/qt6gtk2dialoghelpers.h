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

#ifndef QGTK2DIALOGHELPERS_P_H
#define QGTK2DIALOGHELPERS_P_H

#include <QHash>
#include <QList>
#include <QUrl>
#include <QScopedPointer>
#include <QString>
#include <qpa/qplatformdialoghelper.h>

typedef struct _GtkWidget GtkWidget;
typedef struct _GtkDialog GtkDialog;
typedef struct _GtkFileFilter GtkFileFilter;

QT_BEGIN_NAMESPACE

class QGtk2Dialog;
class QColor;

class Qt6Gtk2ColorDialogHelper : public QPlatformColorDialogHelper
{
    Q_OBJECT

public:
    Qt6Gtk2ColorDialogHelper();
    ~Qt6Gtk2ColorDialogHelper();

    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent) override;
    void exec() override;
    void hide() override;

    void setCurrentColor(const QColor &color) override;
    QColor currentColor() const override;

private Q_SLOTS:
    void onAccepted();

private:
    static void onColorChanged(Qt6Gtk2ColorDialogHelper *helper);
    void applyOptions();

    QScopedPointer<QGtk2Dialog> d;
};

class Qt6Gtk2FileDialogHelper : public QPlatformFileDialogHelper
{
    Q_OBJECT

public:
    Qt6Gtk2FileDialogHelper();
    ~Qt6Gtk2FileDialogHelper();

    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent) override;
    void exec() override;
    void hide() override;

    bool defaultNameFilterDisables() const override;
    void setDirectory(const QUrl &directory) override;
    QUrl directory() const override;
    void selectFile(const QUrl &filename) override;
    QList<QUrl> selectedFiles() const override;
    void setFilter() override;
    void selectNameFilter(const QString &filter) override;
    QString selectedNameFilter() const override;

private Q_SLOTS:
    void onAccepted();

private:
    static void onSelectionChanged(GtkDialog *dialog, Qt6Gtk2FileDialogHelper *helper);
    static void onCurrentFolderChanged(Qt6Gtk2FileDialogHelper *helper);
    static void onUpdatePreview(GtkDialog *dialog, Qt6Gtk2FileDialogHelper *helper);
    void applyOptions();
    void setNameFilters(const QStringList &filters);

    QUrl _dir;
    QList<QUrl> _selection;
    QHash<QString, GtkFileFilter*> _filters;
    QHash<GtkFileFilter*, QString> _filterNames;
    QScopedPointer<QGtk2Dialog> d;
    GtkWidget *previewWidget;
};

class Qt6Gtk2FontDialogHelper : public QPlatformFontDialogHelper
{
    Q_OBJECT

public:
    Qt6Gtk2FontDialogHelper();
    ~Qt6Gtk2FontDialogHelper();

    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent) override;
    void exec() override;
    void hide() override;

    void setCurrentFont(const QFont &font) override;
    QFont currentFont() const override;

private Q_SLOTS:
    void onAccepted();

private:
    void applyOptions();

    QScopedPointer<QGtk2Dialog> d;
};

QT_END_NAMESPACE

#endif // QT5GTK2DIALOGHELPERS_P_H
