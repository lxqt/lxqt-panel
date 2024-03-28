#pragma once

#include <QIcon>
#include <QPointer>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>

#include "qwayland-plasma-window-management.h"

typedef quintptr WId;

class LXQtTaskBarPlasmaWindowManagment;

class LXQtTaskBarPlasmaWindow : public QObject,
                                public QtWayland::org_kde_plasma_window
{
    Q_OBJECT
public:
    LXQtTaskBarPlasmaWindow(const QString &uuid, ::org_kde_plasma_window *id);
    ~LXQtTaskBarPlasmaWindow();

    inline WId getWindowId() const { return reinterpret_cast<WId>(this); }

    using state = QtWayland::org_kde_plasma_window_management::state;
    const QString uuid;
    QString title;
    QString appId;
    QIcon icon;
    QFlags<state> windowState;
    QList<QString> virtualDesktops;
    QRect geometry;
    QString applicationMenuService;
    QString applicationMenuObjectPath;
    QList<QString> activities;
    quint32 pid;
    QString resourceName;
    QPointer<LXQtTaskBarPlasmaWindow> parentWindow;
    bool wasUnmapped = false;
    bool acceptedInTaskBar = false;

Q_SIGNALS:
    void unmapped();
    void titleChanged();
    void appIdChanged();
    void iconChanged();
    void activeChanged();
    void minimizedChanged();
    void maximizedChanged();
    void fullscreenChanged();
    void keepAboveChanged();
    void keepBelowChanged();
    void onAllDesktopsChanged();
    void demandsAttentionChanged();
    void closeableChanged();
    void minimizeableChanged();
    void maximizeableChanged();
    void fullscreenableChanged();
    void skiptaskbarChanged();
    void shadeableChanged();
    void shadedChanged();
    void movableChanged();
    void resizableChanged();
    void virtualDesktopChangeableChanged();
    void skipSwitcherChanged();
    void virtualDesktopEntered();
    void virtualDesktopLeft();
    void geometryChanged();
    void skipTaskbarChanged();
    void applicationMenuChanged();
    void activitiesChanged();
    void parentWindowChanged();
    void initialStateDone();

protected:
    void org_kde_plasma_window_unmapped() override;
    void org_kde_plasma_window_title_changed(const QString &title) override;
    void org_kde_plasma_window_app_id_changed(const QString &app_id) override;
    void org_kde_plasma_window_icon_changed() override;
    void org_kde_plasma_window_themed_icon_name_changed(const QString &name) override;
    void org_kde_plasma_window_state_changed(uint32_t flags) override;
    void org_kde_plasma_window_virtual_desktop_entered(const QString &id) override;

    void org_kde_plasma_window_virtual_desktop_left(const QString &id) override;
    void org_kde_plasma_window_geometry(int32_t x, int32_t y, uint32_t width, uint32_t height) override;
    void org_kde_plasma_window_application_menu(const QString &service_name, const QString &object_path) override;
    void org_kde_plasma_window_activity_entered(const QString &id) override;
    void org_kde_plasma_window_activity_left(const QString &id) override;
    void org_kde_plasma_window_pid_changed(uint32_t pid) override;
    void org_kde_plasma_window_resource_name_changed(const QString &resource_name) override;
    void org_kde_plasma_window_parent_window(::org_kde_plasma_window *parent) override;
    void org_kde_plasma_window_initial_state() override;

private:
    void setParentWindow(LXQtTaskBarPlasmaWindow *parent);

    QMetaObject::Connection parentWindowUnmappedConnection;
};

class LXQtTaskBarPlasmaWindowManagment : public QWaylandClientExtensionTemplate<LXQtTaskBarPlasmaWindowManagment>,
                                         public QtWayland::org_kde_plasma_window_management
{
    Q_OBJECT
public:
    static constexpr int version = 16;

    LXQtTaskBarPlasmaWindowManagment();
    ~LXQtTaskBarPlasmaWindowManagment();

    inline bool isShowingDesktop() const { return m_isShowingDesktop; }

protected:
    void org_kde_plasma_window_management_show_desktop_changed(uint32_t state) override;
    void org_kde_plasma_window_management_window_with_uuid(uint32_t id, const QString &uuid) override;
    void org_kde_plasma_window_management_stacking_order_uuid_changed(const QString &uuids) override;

Q_SIGNALS:
    void windowCreated(LXQtTaskBarPlasmaWindow *window);
    void stackingOrderChanged(const QString &uuids);

private:
    bool m_isShowingDesktop = false;
};

// class Q_DECL_HIDDEN WaylandTasksModel::Private
// {
// public:
//     Private(WaylandTasksModel *q);
//     QHash<PlasmaWindow *, AppData> appDataCache;
//     QHash<PlasmaWindow *, QTime> lastActivated;
//     PlasmaWindow *activeWindow = nullptr;
//     std::vector<std::unique_ptr<PlasmaWindow>> windows;
//     // key=transient child, value=leader
//     QHash<PlasmaWindow *, PlasmaWindow *> transients;
//     // key=leader, values=transient children
//     QMultiHash<PlasmaWindow *, PlasmaWindow *> transientsDemandingAttention;
//     std::unique_ptr<PlasmaWindowManagement> windowManagement;
//     KSharedConfig::Ptr rulesConfig;
//     KDirWatch *configWatcher = nullptr;
//     VirtualDesktopInfo *virtualDesktopInfo = nullptr;
//     static QUuid uuid;
//     QList<QString> stackingOrder;

//     void init();
//     void initWayland();
//     auto findWindow(PlasmaWindow *window) const;
//     void addWindow(PlasmaWindow *window);

//     const AppData &appData(PlasmaWindow *window);

//     QIcon icon(PlasmaWindow *window);

//     static QString mimeType();
//     static QString groupMimeType();

//     void dataChanged(PlasmaWindow *window, int role);
//     void dataChanged(PlasmaWindow *window, const QList<int> &roles);

// private:
//     WaylandTasksModel *q;
// };
