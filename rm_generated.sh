#!/bin/sh
#
# run this after a "make clean"
# removes further generated files 
#
rm -f   Makefile.in \
        QT/Makefile.in \
        QT/PstoeditQtGui/Makefile.in \
        aclocal.m4 \
        ar-lib \
        compile \
        config.guess \
        config.sub \
        config/Makefile.in \
        configure \
        contrib/Makefile.in \
        contrib/cairo/Makefile.in \
        contrib/java/Makefile.in \
        contrib/java/java1/Makefile.in \
        contrib/java/java2/Makefile.in \
        depcomp \
        libtool \
        doc/Makefile.in \
        doc/pstoedit.1 \
        doc/pstoedit.pdf \
        examples/Makefile.in \
        install-sh \
        ltmain.sh \
        misc/Makefile.in \
        misc/swffonts/Makefile.in \
        missing \
        mkinstalldirs \
        msdev/Makefile.in \
        othersrc/Makefile.in \
        othersrc/gsdllinc/Makefile.in \
        src/Makefile.in \
        src/pstoedit.ph \
	src/pstoedit_config.h.in \
	src/pstoedit_config.h 

rm -f \
	config.log \
	config.status \
	config/pstoedit.pc \
	doc/version.tex \
	src/stamp-h1 \
	QT/PstoeditQtGui/Makefile.qt \
	QT/PstoeditQtGui/.qmake.stash \
	`find . -name Makefile`


rm -rf  autom4te.cache \
        m4 \
	src/.deps
