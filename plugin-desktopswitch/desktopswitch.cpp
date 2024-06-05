/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include <QLabel>
#include <QButtonGroup>
#include <QWheelEvent>
#include <QtDebug>
#include <QSignalMapper>
#include <QTimer>
#include <lxqt-globalkeys.h>
#include <LXQt/GridLayout>

#include "../panel/lxqtpanelapplication.h"
#include "../panel/backends/ilxqttaskbarabstractbackend.h"

#include <cmath>

#include "desktopswitch.h"
#include "desktopswitchbutton.h"
#include "desktopswitchconfiguration.h"

static const QString DEFAULT_SHORTCUT_TEMPLATE(QStringLiteral("Control+F%1"));

DesktopSwitch::DesktopSwitch(const ILXQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    m_pSignalMapper(new QSignalMapper(this)),
    m_desktopCount(0),
    mRows(-1),
    mShowOnlyActive(false),
    mDesktops(nullptr),
    mLabelType(DesktopSwitchButton::LABEL_TYPE_INVALID)
{
    LXQtPanelApplication *a = reinterpret_cast<LXQtPanelApplication*>(qApp);
    mBackend = a->getWMBackend();


    m_desktopCount = mBackend->getWorkspacesCount();

    m_buttons = new QButtonGroup(this);

    connect (m_pSignalMapper, &QSignalMapper::mappedInt, this, &DesktopSwitch::setDesktop);


    mLayout = new LXQt::GridLayout(&mWidget);
    mWidget.setLayout(mLayout);

    settingsChanged();

    onCurrentDesktopChanged(mBackend->getCurrentWorkspace());
    QTimer::singleShot(0, this, SLOT(registerShortcuts()));

    connect(m_buttons, &QButtonGroup::idClicked, this, &DesktopSwitch::setDesktop);

    connect(mBackend, &ILXQtTaskbarAbstractBackend::workspacesCountChanged,  this, &DesktopSwitch::onNumberOfDesktopsChanged);
    connect(mBackend, &ILXQtTaskbarAbstractBackend::currentWorkspaceChanged, this, &DesktopSwitch::onCurrentDesktopChanged);
    connect(mBackend, &ILXQtTaskbarAbstractBackend::workspaceNameChanged,    this, &DesktopSwitch::onDesktopNamesChanged);

    connect(mBackend, &ILXQtTaskbarAbstractBackend::windowPropertyChanged, this, &DesktopSwitch::onWindowChanged);
}

void DesktopSwitch::registerShortcuts()
{
    // Register shortcuts to change desktop
    GlobalKeyShortcut::Action * gshortcut;
    QString path;
    QString description;
    for (int i = 0; i < 12; ++i)
    {
        path = QStringLiteral("/panel/%1/desktop_%2").arg(settings()->group()).arg(i + 1);
        description = tr("Switch to desktop %1").arg(i + 1);

        gshortcut = GlobalKeyShortcut::Client::instance()->addAction(QString(), path, description, this);
        if (nullptr != gshortcut)
        {
            m_keys << gshortcut;
            connect(gshortcut, &GlobalKeyShortcut::Action::registrationFinished, this, &DesktopSwitch::shortcutRegistered);
            connect(gshortcut, &GlobalKeyShortcut::Action::activated, m_pSignalMapper, [this] {
                m_pSignalMapper->map();
            });
            m_pSignalMapper->setMapping(gshortcut, i);
        }
    }
}

void DesktopSwitch::shortcutRegistered()
{
    GlobalKeyShortcut::Action * const shortcut = qobject_cast<GlobalKeyShortcut::Action*>(sender());

    disconnect(shortcut, &GlobalKeyShortcut::Action::registrationFinished, this, &DesktopSwitch::shortcutRegistered);

    const int i = m_keys.indexOf(shortcut);
    Q_ASSERT(-1 != i);

    if (shortcut->shortcut().isEmpty())
    {
        shortcut->changeShortcut(DEFAULT_SHORTCUT_TEMPLATE.arg(i + 1));
    }
}

void DesktopSwitch::onWindowChanged(WId id, int prop)
{
    if (prop == int(LXQtTaskBarWindowProperty::State))
    {
        int desktop = mBackend->getWindowWorkspace(id);
        if (desktop == int(LXQtTaskBarWorkspace::ShowOnAll))
            return;
        else
        {
            DesktopSwitchButton *button = static_cast<DesktopSwitchButton *>(m_buttons->button(desktop - 1));
            if(button)
                button->setUrgencyHint(id, mBackend->applicationDemandsAttention(id));
        }
    }
}

void DesktopSwitch::refresh()
{
    const QList<QAbstractButton*> btns = m_buttons->buttons();

    int i = 0;
    const int current_desktop = mBackend->getCurrentWorkspace();
    const int current_cnt = btns.count();
    const int border = qMin(btns.count(), m_desktopCount);
    //update existing buttons
    for ( ; i < border; ++i)
    {
        DesktopSwitchButton * button = qobject_cast<DesktopSwitchButton*>(btns[i]);
        button->update(i, mLabelType,
                       mBackend->getWorkspaceName(i + 1).isEmpty() ?
                       tr("Desktop %1").arg(i + 1) :
                       mBackend->getWorkspaceName(i + 1));
        button->setVisible(!mShowOnlyActive || i + 1 == current_desktop);
    }

    //create new buttons (if necessary)
    QAbstractButton *b;
    for ( ; i < m_desktopCount; ++i)
    {
        b = new DesktopSwitchButton(&mWidget, i, mLabelType,
                mBackend->getWorkspaceName(i+1).isEmpty() ?
                tr("Desktop %1").arg(i+1) :
                mBackend->getWorkspaceName(i+1));
        mWidget.layout()->addWidget(b);
        m_buttons->addButton(b, i);
        b->setVisible(!mShowOnlyActive || i + 1 == current_desktop);
    }

    //delete unneeded buttons (if necessary)
    for ( ; i < current_cnt; ++i)
    {
        b = m_buttons->buttons().constLast();
        m_buttons->removeButton(b);
        mWidget.layout()->removeWidget(b);
        delete b;
    }
}

DesktopSwitch::~DesktopSwitch() = default;

void DesktopSwitch::setDesktop(int desktop)
{
    mBackend->setCurrentWorkspace(desktop + 1);
}

void DesktopSwitch::onNumberOfDesktopsChanged()
{
    int count = mBackend->getWorkspacesCount();
    qDebug() << "Desktop count changed from" << m_desktopCount << "to" << count;
    m_desktopCount = count;
    refresh();
}

void DesktopSwitch::onCurrentDesktopChanged(int current)
{
    if (mShowOnlyActive)
    {
        mLayout->setEnabled(false);
        int i = 1;
        const auto buttons = m_buttons->buttons();
        for (const auto button : buttons)
        {
            if (current == i)
            {
                button->setChecked(true);
                button->setVisible(true);
            } else
            {
                button->setVisible(false);
            }
            ++i;
        }
        mLayout->setEnabled(true);
    } else
    {
        QAbstractButton *button = m_buttons->button(current - 1);
        if (button)
            button->setChecked(true);
    }
}

void DesktopSwitch::onDesktopNamesChanged()
{
    refresh();
}

void DesktopSwitch::settingsChanged()
{
    const int rows = settings()->value(QStringLiteral("rows"), 1).toInt();
    const bool show_only_active = settings()->value(QStringLiteral("showOnlyActive"), false).toBool();
    const int label_type = settings()->value(QStringLiteral("labelType"), DesktopSwitchButton::LABEL_TYPE_NUMBER).toInt();

    const bool need_realign = mRows != rows || show_only_active != mShowOnlyActive;
    const bool need_refresh = mLabelType != static_cast<DesktopSwitchButton::LabelType>(label_type) || show_only_active != mShowOnlyActive;

    mRows = rows;
    mShowOnlyActive = show_only_active;
    mLabelType = static_cast<DesktopSwitchButton::LabelType>(label_type);
    if (need_realign)
    {
        // WARNING: Changing the desktop layout may call "LXQtPanel::realign", which calls
        // "DesktopSwitch::realign()". Therefore, the desktop layout should not be changed
        // inside the latter method.
        int columns = static_cast<int>(ceil(static_cast<float>(m_desktopCount) / mRows));
        mBackend->setDesktopLayout(panel()->isHorizontal() ? Qt::Horizontal : Qt::Vertical,
                                   mRows, columns, mWidget.isRightToLeft());

        realign(); // in case it isn't called when the desktop layout changes
    }
    if (need_refresh)
        refresh();
}

void DesktopSwitch::realign()
{
    mLayout->setEnabled(false);
    if (panel()->isHorizontal())
    {
        mLayout->setRowCount(mShowOnlyActive ? 1 : mRows);
        mLayout->setColumnCount(0);
    }
    else
    {
        mLayout->setColumnCount(mShowOnlyActive ? 1 : mRows);
        mLayout->setRowCount(0);
    }
    mLayout->setEnabled(true);
}

QDialog *DesktopSwitch::configureDialog()
{
    return new DesktopSwitchConfiguration(settings());
}

DesktopSwitchWidget::DesktopSwitchWidget():
    QFrame(),
    m_mouseWheelThresholdCounter(0)
{
}

void DesktopSwitchWidget::wheelEvent(QWheelEvent *e)
{
    // Without some sort of threshold which has to be passed, scrolling is too sensitive
    QPoint angleDelta = e->angleDelta();
    Qt::Orientation orient = (qAbs(angleDelta.x()) > qAbs(angleDelta.y()) ? Qt::Horizontal : Qt::Vertical);
    int rotationSteps = (orient == Qt::Horizontal ? angleDelta.x() : angleDelta.y());

    m_mouseWheelThresholdCounter -= rotationSteps;

    // If the user hasn't scrolled far enough in one direction (positive or negative): do nothing
    if(abs(m_mouseWheelThresholdCounter) < 100)
        return;

    LXQtPanelApplication *a = reinterpret_cast<LXQtPanelApplication*>(qApp);
    auto wmBackend = a->getWMBackend();

    int max = wmBackend->getWorkspacesCount();
    int delta = rotationSteps < 0 ? 1 : -1;
    int current = wmBackend->getCurrentWorkspace() + delta;

    if (current > max){
        current = 1;
    }
    else if (current < 1)
        current = max;

    m_mouseWheelThresholdCounter = 0;
    wmBackend->setCurrentWorkspace(current);
}

ILXQtPanelPlugin *DesktopSwitchPluginLibrary::instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
{
    LXQtPanelApplication *a = reinterpret_cast<LXQtPanelApplication*>(qApp);
    auto wmBackend = a ? a->getWMBackend() : nullptr;
    if(!wmBackend || !wmBackend->supportsAction(0, LXQtTaskBarBackendAction::DesktopSwitch))
        return new DesktopSwitchUnsupported{startupInfo};

    return new DesktopSwitch{startupInfo};
}

DesktopSwitchUnsupported::DesktopSwitchUnsupported(const ILXQtPanelPluginStartupInfo &startupInfo)
    : ILXQtPanelPlugin(startupInfo)
    , mLabel(new QLabel(tr("n/a")))
{
    mLabel->setToolTip(tr("DesktopSwitch is unsupported on current platform: %1").arg(QGuiApplication::platformName()));
}

DesktopSwitchUnsupported::~DesktopSwitchUnsupported()
{
    delete mLabel;
    mLabel = nullptr;
}

QWidget *DesktopSwitchUnsupported::widget()
{
    return mLabel;
}
