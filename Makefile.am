ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST=LICENSE README.md autogen.sh rm_generated.sh
SUBDIRS = config src doc examples contrib misc othersrc 
SUBDIRS += msdev QT 
m4datadir = $(datadir)/aclocal
# m4data_DATA = m4/pstoedit.m4
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool
