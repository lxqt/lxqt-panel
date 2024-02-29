set -ex

#source shared-ci/prepare-archlinux.sh

cat >> /etc/pacman.conf <<EOF
[lxqt2wip]
SigLevel = Optional TrustAll
Server = https://archlive-aur.sourceforge.io/lxqt2wip/x86_64
EOF

# Needed to allow updating archlinux-keyring package (see below).
# This also enables usage of third-party keys [1].
pacman-key --init

# Update the keyring first to allow installing packages signed by newer keys [2].
# This may be needed when `pacman -Syu` below or other pacman commands in individual LXQt
# components uses packages signed by newer keys.
pacman -Sy --noconfirm archlinux-keyring

pacman -Syu --noconfirm

# [1] https://gitlab.archlinux.org/archlinux/archlinux-docker/-/issues/18
# [2] https://github.com/lxqt/lxqt-panel/pull/1781#issuecomment-1111755255

# See *depends in https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=lxqt-panel-git
packages=(
    alsa-lib
    cmake
    git
    libpulse
    libstatgrab
    libxcomposite
    libxtst
    lm_sensors
    menu-cache
    qt6-base
    qt6-tools
#   kwindowsystem
#   solid
)
pacman -S --noconfirm --needed ${packages[@]}

# Enable testing repo only to checkout KF6Solid, KF6WindowSystem and layer-shell-qt
cat >> /etc/pacman.conf <<EOF
[core-testing]
Include = /etc/pacman.d/mirrorlist
[extra-testing]
Include = /etc/pacman.d/mirrorlist
EOF
pacman -Sy --noconfirm

lxqt2wip_packages=(
    layer-shell-qt
    kwindowsystem
    solid
    libdbusmenu-lxqt-git
    lxqt2-build-tools-git
    lxqt-menu-data-git
    libqt6xdg-git
    liblxqt-git
    lxqt-globalkeys-git
    libsysstat-git
)
pacman -S --noconfirm ${lxqt2wip_packages[@]}

# workaround
cd /usr/share/cmake
cp -R lxqt2-menu-data lxqt-menu-data
cd lxqt-menu-data
mv lxqt2-menu-data-config.cmake lxqt-menu-data-config.cmake
mv lxqt2-menu-data-config-version.cmake lxqt-menu-data-config-version.cmake
