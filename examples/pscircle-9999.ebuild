# Copyright 1999-2018 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6

DESCRIPTION="pscircle visualizes Linux processes in a form of radial tree"
HOMEPAGE="https://gitlab.com/mildlyparallel/pscircle"
EGIT_REPO_URI="${HOMEPAGE}.git"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~alpha ~amd64 ~arm ~hppa ~ia64 ~ppc ~ppc64 ~s390 ~sh ~sparc ~x86"
IUSE=""

DEPEND="x11-libs/cairo"
RDEPEND="${DEPEND}
	media-libs/libpng
	x11-libs/libX11
"

inherit git-r3 meson
