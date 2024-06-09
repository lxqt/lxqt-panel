#pragma once

#include <QVariant>
#include <vector>

class LXQtWlrootsVirtualDesktop : public QObject
{
    Q_OBJECT
public:
    LXQtWlrootsVirtualDesktop(const QString &id);
    ~LXQtWlrootsVirtualDesktop();
    const QString id;
    QString name;
Q_SIGNALS:
    void done();
    void activated();
    void nameChanged();
};

class LXQtWlrootsVirtualDesktopManagment : public QObject
{
    Q_OBJECT
public:
    LXQtWlrootsVirtualDesktopManagment();
    ~LXQtWlrootsVirtualDesktopManagment();

signals:
    void desktopCreated(const QString &id, quint32 position);
    void desktopRemoved(const QString &id);
    void rowsChanged(const quint32 rows);
};

class Q_DECL_HIDDEN LXQtWlrootsWaylandWorkspaceInfo : public QObject
{
    Q_OBJECT
public:
    LXQtWlrootsWaylandWorkspaceInfo();

    QVariant currentVirtualDesktop;
    std::vector<std::unique_ptr<LXQtWlrootsVirtualDesktop>> virtualDesktops;
    std::unique_ptr<LXQtWlrootsVirtualDesktopManagment> virtualDesktopManagement;
    const quint32 rows = 1;

    typedef std::vector<std::unique_ptr<LXQtWlrootsVirtualDesktop>>::const_iterator VirtualDesktopsIterator;

    VirtualDesktopsIterator findDesktop(const QString &id) const;

    QString getDesktopName(int pos) const;
    QString getDesktopId(int pos) const;

    void init();
    void addDesktop(const QString &id, quint32 pos);
    QVariant currentDesktop() const;
    int numberOfDesktops() const;
    QVariantList desktopIds() const;
    QStringList desktopNames() const;
    quint32 position(const QVariant &desktop) const;
    int desktopLayoutRows() const;
    void requestActivate(const QVariant &desktop);
    void requestCreateDesktop(quint32 position);
    void requestRemoveDesktop(quint32 position);

signals:
    void currentDesktopChanged();
    void numberOfDesktopsChanged();
    void navigationWrappingAroundChanged();
    void desktopIdsChanged();
    void desktopNameChanged(quint32 position);
    void desktopLayoutRowsChanged();
};
