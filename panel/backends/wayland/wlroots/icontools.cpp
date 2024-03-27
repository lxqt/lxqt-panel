#include <filesystem>

#include "icontools.hpp"

static inline QString getPixmapIcon( QString name ) {
    QStringList paths{
        QString::fromUtf8( "/usr/local/share/pixmaps/" ),
        QString::fromUtf8( "/usr/share/pixmaps/" ),
    };

    QStringList sfxs{
        QString::fromUtf8( ".svg" ), QString::fromUtf8( ".png" ), QString::fromUtf8( ".xpm" )
    };

    for ( QString path: paths ) {
        for ( QString sfx: sfxs ) {
            if ( QFile::exists( path + name + sfx ) ) {
                return path + name + sfx;
            }
        }
    }

    return QString();
}


QIcon getIconForAppId( QString mAppId ) {
    if ( mAppId.isEmpty() or (mAppId == QString::fromUtf8("Unknown")) ) {
        return QIcon();
    }

    /** Wine apps */
    if ( mAppId.endsWith( QString::fromUtf8(".exe") ) ) {
        return QIcon::fromTheme( QString::fromUtf8("wine") );
    }

    /** Check if a theme icon exists called @mAppId */
    if ( QIcon::hasThemeIcon( mAppId ) ) {
        return QIcon::fromTheme( mAppId );
    }

    /** Check if the theme icon is @mAppId, but all lower-case letters */
    else if ( QIcon::hasThemeIcon( mAppId.toLower() ) ) {
        return QIcon::fromTheme( mAppId.toLower() );
    }

    QStringList appDirs = {
        QDir::home().filePath( QString::fromUtf8( ".local/share/applications/" ) ),
        QString::fromUtf8( "/usr/local/share/applications/" ),
        QString::fromUtf8( "/usr/share/applications/" ),
        QString::fromUtf8( "/usr/local/share/games/" ),
        QString::fromUtf8( "/usr/share/games/" ),
    };

    /**
     * Assume mAppId == desktop-file-name (ideal situation)
     * or mAppId.toLower() == desktop-file-name (cheap fallback)
     */
    QString iconName;

    for ( QString path: appDirs ) {
        /** Get the icon name from desktop (mAppId: as it is) */
        if ( QFile::exists( path + mAppId + QString::fromUtf8(".desktop") ) ) {
            QSettings desktop( path + mAppId + QString::fromUtf8(".desktop"), QSettings::IniFormat );
            iconName = desktop.value( QString::fromUtf8( "Desktop Entry/Icon" ) ).toString();
        }

        /** Get the icon name from desktop (mAppId: all lower-case letters) */
        else if ( QFile::exists( path + mAppId.toLower() + QString::fromUtf8(".desktop") ) ) {
            QSettings desktop( path + mAppId.toLower() + QString::fromUtf8(".desktop"), QSettings::IniFormat );
            iconName = desktop.value( QString::fromUtf8("Desktop Entry/Icon") ).toString();
        }

        /** No icon specified: try else-where */
        if ( iconName.isEmpty() ) {
            continue;
        }

        /** We got an iconName, and it's in the current theme */
        if ( QIcon::hasThemeIcon( iconName ) ) {
            return QIcon::fromTheme( iconName );
        }

        /** Not a theme icon, but an absolute path */
        else if ( QFile::exists( iconName ) ) {
            return QIcon( iconName );
        }

        /** Not theme icon or absolute path. So check /usr/share/pixmaps/ */
        else {
            iconName = getPixmapIcon( iconName );

            if ( not iconName.isEmpty() ) {
                return QIcon( iconName );
            }
        }
    }

    /* Check all desktop files for @mAppId */
    for ( QString path: appDirs ) {
        QStringList desktops = QDir( path ).entryList( { QString::fromUtf8( "*.desktop" ) } );
        for ( QString dskf: desktops ) {
            QSettings desktop( path + dskf, QSettings::IniFormat );

            QString exec = desktop.value( QString::fromUtf8("Desktop Entry/Exec"), QString::fromUtf8("abcd1234/-") ).toString();
            QString name = desktop.value( QString::fromUtf8("Desktop Entry/Name"), QString::fromUtf8("abcd1234/-") ).toString();
            QString cls  = desktop.value( QString::fromUtf8("Desktop Entry/StartupWMClass"), QString::fromUtf8("abcd1234/-") ).toString();

            QString execPath( QString::fromUtf8( std::filesystem::path( exec.toStdString() ).filename().c_str() ) );

            if ( mAppId.compare( execPath, Qt::CaseInsensitive ) == 0 ) {
                iconName = desktop.value( QString::fromUtf8("Desktop Entry/Icon") ).toString();
            }

            else if ( mAppId.compare( name, Qt::CaseInsensitive ) == 0 ) {
                iconName = desktop.value( QString::fromUtf8("Desktop Entry/Icon") ).toString();
            }

            else if ( mAppId.compare( cls, Qt::CaseInsensitive ) == 0 ) {
                iconName = desktop.value( QString::fromUtf8("Desktop Entry/Icon") ).toString();
            }

            if ( not iconName.isEmpty() ) {
                if ( QIcon::hasThemeIcon( iconName ) ) {
                    return QIcon::fromTheme( iconName );
                }

                else if ( QFile::exists( iconName ) ) {
                    return QIcon( iconName );
                }

                else {
                    iconName = getPixmapIcon( iconName );

                    if ( not iconName.isEmpty() ) {
                        return QIcon( iconName );
                    }
                }
            }
        }
    }

    iconName = getPixmapIcon( iconName );

    if ( not iconName.isEmpty() ) {
        return QIcon( iconName );
    }

    return QIcon();
}
