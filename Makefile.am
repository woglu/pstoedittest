ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST=copying autogen.sh readme.install m4/pstoedit.m4
SUBDIRS = config src doc examples contrib misc othersrc 
SUBDIRS += msdev QT 
#SUBDIRS += os2 
m4datadir = $(datadir)/aclocal
###m4data_DATA = m4/pstoedit.m4
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool
