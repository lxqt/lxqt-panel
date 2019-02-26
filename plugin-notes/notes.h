/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Matteo Fois <giomatfois62@yahoo.it>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef LXQT_NOTES_H
#define LXQT_NOTES_H

#include "../panel/ilxqtpanelplugin.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFontMetrics>
#include <QTextEdit>
#include <QToolButton>
#include <QList>
#include <XdgIcon>

class StickyNote;

class Notes : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    explicit Notes(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~Notes();

    virtual QWidget *widget() { return &mButton; }
    virtual QString themeId() const { return "Notes"; }
    virtual ILXQtPanelPlugin::Flags flags() const { return HaveConfigDialog; }

    bool isSeparate() const { return true; }
    QDialog *configureDialog();

public slots:
    void realign();
    void toggleShowHide();
    void addNewNote();
    void deleteNote(const qint64 &id);
    
protected slots:
    void settingsChanged();

private:
    QToolButton mButton;
    bool mHidden;
    QMap<qint64, StickyNote*> mNotes;
    QString dataDir(); // (cannot be static for some reason)
    
    void setIconsColor(const QString &color);
    void setIconColor(const QString &icon, const QString &color);
};


class NotesLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        return new Notes(startupInfo);
    }
};

#endif
