/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012-2013 Razor team
 *            2014 LXQt team
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
class LXQtWorldClockPopup;


class LXQtWorldClock : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    LXQtWorldClock(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~LXQtWorldClock() override;

    QWidget *widget() override { return mMainWidget; }
    QString themeId() const override { return QLatin1String("WorldClock"); }
    ILXQtPanelPlugin::Flags flags() const override { return PreferRightAlignment | HaveConfigDialog ; }
    bool isSeparate() const override { return true; }
    void activated(ActivationReason reason) override;

    void settingsChanged() override;
    void realign() override;
    QDialog *configureDialog() override;
    bool eventFilter(QObject * watched, QEvent * event) override;

private slots:
    void timeout();
    void wheelScrolled(int);
    void deletePopup();
    void updateTimeText();

private:
    QWidget *mMainWidget;
    LXQt::RotatedWidget* mRotatedWidget;
    ActiveLabel *mContent;
    LXQtWorldClockPopup* mPopup;

    QTimer *mTimer;
    int mUpdateInterval;

    QStringList mTimeZones;
    QMap<QString, QString> mTimeZoneCustomNames;
    QString mDefaultTimeZone;
    QString mActiveTimeZone;
    QString mFormat;

    bool mAutoRotate;
    bool mShowWeekNumber;
    bool mShowTooltip;
    QLabel *mPopupContent;

    QDateTime mShownTime;

    void restartTimer();

    void setTimeText();
    QString formatDateTime(const QDateTime &datetime, const QString &timeZoneName);
    void updatePopupContent();
    bool formatHasTimeZone(QString format);
    QString preformat(const QString &format, const QTimeZone &timeZone, const QDateTime& dateTime);
};


class ActiveLabel : public QLabel
{
Q_OBJECT

public:
    explicit ActiveLabel(QWidget * = nullptr);

signals:
    void wheelScrolled(int);
    void leftMouseButtonClicked();
    void middleMouseButtonClicked();

protected:
    void wheelEvent(QWheelEvent *);
    void mouseReleaseEvent(QMouseEvent* event);
};

class LXQtWorldClockPopup : public QDialog
{
    Q_OBJECT

public:
    LXQtWorldClockPopup(QWidget *parent = nullptr);

    void show();

signals:
    void deactivated();

protected:
    bool event(QEvent* ) override;

};

class LXQtWorldClockLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LXQtWorldClock(startupInfo);
    }
};

#endif // LXQT_PANEL_WORLDCLOCK_H
