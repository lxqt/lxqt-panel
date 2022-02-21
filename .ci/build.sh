set -ex

source shared-ci/prepare-archlinux.sh

# See *depends in https://github.com/archlinuxcn/repo/blob/master/archlinuxcn/lxqt-panel-git/PKGBUILD
pacman -S --noconfirm --needed git cmake qt5-tools lxqt-build-tools-git alsa-lib libpulse lm_sensors libstatgrab libsysstat-git solid menu-cache libxcomposite lxmenu-data libdbusmenu-qt5 lxqt-globalkeys-git libxtst

cmake -B build -S .
make -C build
