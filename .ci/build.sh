set -ex

source shared-ci/prepare-archlinux.sh

# See *depends in https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=lxqt-panel-git
pacman -S --noconfirm --needed git cmake qt6-base qt6-tools lxqt-build-tools-git alsa-lib libpulse lm_sensors libstatgrab libsysstat-git solid menu-cache libxcomposite lxqt-menu-data-git libdbusmenu-lxqt-git lxqt-globalkeys-git libxtst

cmake -B build -S .
make -C build
