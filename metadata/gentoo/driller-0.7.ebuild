# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

DESCRIPTION="Extracts data from binary files"
HOMEPAGE="http://66.122.11.187/driller/"
SRC_URI="${HOMEPAGE}/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86"
IUSE="debug mysql qt"

DEPEND="
	virtual/libc
	dev-libs/libxml2
	mysql? ( dev-db/mysql )
	qt? ( >=x11-libs/qt-4.0.0 )"

src_compile() {
	econf \
		$(use_enable debug) \
		$(use_enable mysql) \
		$(use_enable qt) || die
	emake || die
}

src_install() {
	emake DESTDIR=${D} install || die
}
