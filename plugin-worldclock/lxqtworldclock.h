/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2012-2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#ifndef LXQT_PANEL_WORLDCLOCK_H
#define LXQT_PANEL_WORLDCLOCK_H

#include <QTimeZone>

#include <QDialog>
#include <QLabel>

#include <LXQt/RotatedWidget>

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtworldclockconfiguration.h"


class ActiveLabel;
class QTimer;
class LxQtWorldClockPopup;


class LxQtWorldClock : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    LxQtWorldClock(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtWorldClock();

    virtual QWidget *widget() { return mMainWidget; }
    virtual QString themeId() const { return QLatin1String("WorldClock"); }
    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
    bool isSeparate() const { return true; }
    void activated(ActivationReason reason);

    virtual void settingsChanged();
    virtual void realign();
    QDialog *configureDialog();

private slots:
    void timeout();
    void wheelScrolled(int);
    void deletePopup();

private:
    QWidget *mMainWidget;
    LxQt::RotatedWidget* mRotatedWidget;
    ActiveLabel *mContent;
    LxQtWorldClockPopup* mPopup;

    QTimer *mTimer;
    int mUpdateInterval;
    qint64 mLastUpdate;

    QStringList mTimeZones;
    QMap<QString, QString> mTimeZoneCustomNames;
    QString mDefaultTimeZone;
    QString mActiveTimeZone;
    QString mFormat;

    bool mAutoRotate;
    QLabel *mPopupContent;

    void restartTimer(int);

    QString formatDateTime(const QDateTime &datetime, const QString &timeZoneName);
    void updatePopupContent();
    bool formatHasTimeZone(QString format);
    QString preformat(const QString &format, const QTimeZone &timeZone, const QDateTime& dateTime);
};


class ActiveLabel : public QLabel
{
Q_OBJECT

public:
    explicit ActiveLabel(QWidget * = NULL);

signals:
    void wheelScrolled(int);
    void leftMouseButtonClicked();
    void middleMouseButtonClicked();

protected:
    void wheelEvent(QWheelEvent *);
    void mouseReleaseEvent(QMouseEvent* event);
};

class LxQtWorldClockPopup : public QDialog
{
    Q_OBJECT

public:
    LxQtWorldClockPopup(QWidget *parent = 0);

    void show();

signals:
    void deactivated();

protected:
    virtual bool event(QEvent* );

};

class LxQtWorldClockLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LxQtWorldClock(startupInfo);
    }
};

#endif // LXQT_PANEL_WORLDCLOCK_H
