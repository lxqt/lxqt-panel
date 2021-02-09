set -ex

cat >> /etc/pacman.conf <<EOF
[archlinuxcn]
Server = https://repo.archlinuxcn.org/\$arch
SigLevel = Never
EOF

# Work-around the issue with glibc 2.33 on old Docker engines
# Extract files directly as pacman is also affected by the issue
patched_glibc=glibc-linux4-2.33-4-x86_64.pkg.tar.zst
curl -LO https://repo.archlinuxcn.org/x86_64/$patched_glibc
bsdtar -C / -xvf $patched_glibc

pacman -Syu --noconfirm
# See *depends in https://github.com/archlinuxcn/repo/blob/master/archlinuxcn/lxqt-panel-git/PKGBUILD
pacman -S --noconfirm --needed git cmake qt5-tools lxqt-build-tools-git alsa-lib libpulse lm_sensors libstatgrab libsysstat-git solid menu-cache libxcomposite lxmenu-data libdbusmenu-qt5 lxqt-globalkeys-git

cmake -B build -S .
make -C build
