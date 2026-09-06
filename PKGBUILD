# PKGBUILD for kwin-borderglow-git
# Maintainer: Bartek Ludew <bartekldwdev@gmail.com>

_gitname=kwin-borderglow

pkgname=kwin-borderglow-git
pkgver=r42.864b786
pkgrel=1
pkgdesc="A lightweight and customizable gradient border effect for KWin"
arch=('x86_64')
url="https://github.com/bartekldw/borderglow"
license=('GPL-3.0-or-later')
depends=('kwin' 'qt6-base' 'kconfig' 'kcmutils' 'libepoxy' 'ki18n' 'kcoreaddons' 'kwidgetsaddons' 'vulkan-icd-loader')
makedepends=('git' 'cmake' 'extra-cmake-modules' 'qt6-tools' 'vulkan-headers')
provides=("${pkgname%-git}")
conflicts=("${pkgname%-git}")
source=("${pkgname%-git}::git+https://github.com/bartekldw/borderglow.git")
sha256sums=('SKIP')

pkgver() {
  cd "$srcdir/$_gitname"
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
  cmake -B "$srcdir/build" -S "$srcdir/$_gitname" \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -Wno-dev
  cmake --build "$srcdir/build" -j"$(nproc)"
}

package() {
  DESTDIR="$pkgdir" cmake --install "$srcdir/build"
}