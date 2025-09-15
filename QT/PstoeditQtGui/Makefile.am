EXTRA_DIST=PstoeditQtGui.aps PstoeditQtGui.cpp PstoeditQtGui.h PstoeditQtGui.qrc PstoeditQtGui.rc PstoeditQtGui.sln PstoeditQtGui.ui PstoeditQtGui.pro
EXTRA_DIST+=resource.h pstoeditlogo-medium.ico PstoeditQtGuiMain.cpp
EXTRA_DIST+=pstoedit.png PstoeditQtGui.desktop
#EXTRA_DIST+=PstoeditQtGui.vcxproj PstoeditQtGui.vcxproj.filters PstoeditQtGui.vcxproj.user 

PstoeditQtGui_SOURCES=

if HAVE_QT
bin_PROGRAMS=PstoeditQtGui
PstoeditQtGui$(EXEEXT): GUI

clean-generic:
	$(MAKE) -f Makefile.qt clean

distclean-generic:
	$(MAKE) -f Makefile.qt distclean

.PHONY: GUI
GUI:    Makefile.qt
	$(MAKE) -f $<

Makefile.qt: PstoeditQtGui.pro
	qmake6 -o $@ $<

desktopdir = $(datadir)/applications
#desktop_in_files = PstoeditQtGui.desktop.in
desktop_DATA = PstoeditQtGui.desktop
# desktop-file-validate PstoeditQtGui.desktop

# desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
# @INTLTOOL_DESKTOP_RULE@

icons256dir = $(datadir)/icons/hicolor/256x256/apps
dist_icons256_DATA = pstoedit.png
 
endif
