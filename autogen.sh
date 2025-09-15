#!/bin/sh
set -e
if [ ! -f /usr/share/aclocal/pkg.m4 ]; then
	echo "Your system is missing /usr/share/aclocal/pkg.m4"
	echo "Please install the pkg-config package"
else
	LANG=C
	export LANG
	autoreconf --force --install --warnings=all $*
fi
