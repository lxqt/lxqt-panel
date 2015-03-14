/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LxQt team
 * Authors:
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef SPACER_H
#define SPACER_H

#include "../panel/ilxqtpanelplugin.h"
#include <QFrame>


class SpacerWidget : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString type READ getType)
    Q_PROPERTY(QString orientation READ getOrientation)

public:
    const QString& getType() const throw () { return mType; }
    void setType(QString const & type);
    const QString& getOrientation() const throw () { return mOrientation; }
    void setOrientation(QString const & orientation);

private:
    QString mType;
    QString mOrientation;
};

class Spacer :  public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT

public:
    Spacer(const ILxQtPanelPluginStartupInfo &startupInfo);

    virtual QWidget *widget() { return &mSpacer; }
    virtual QString themeId() const { return "Spacer"; }

    bool isSeparate() const { return true; }

    virtual ILxQtPanelPlugin::Flags flags() const { return HaveConfigDialog; }
    QDialog *configureDialog();

    virtual void realign();

private slots:
    virtual void settingsChanged();

private:
    void setSizes();

private:
    SpacerWidget mSpacer;
    int mSize;
};

class SpacerPluginLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const { return new Spacer(startupInfo);}
};

#endif

