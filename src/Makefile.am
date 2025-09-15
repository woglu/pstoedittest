EXTRA_DIST=pstoedit.pro ps_pro_to_ph.awk get_D_and_I_options.sh

if HAVE_PLUGINSRC
#LIBPLUGINS_ENTRY=libp2edrvplugins.la
endif

if HAVE_LIBPLOTTER
LIBPLOTTER_ENTRY=libp2edrvlplot.la
endif

if HAVE_LIBMING
LIBMING_ENTRY=libp2edrvswf.la
endif

if HAVE_LIBZIP
LIBPPTX_ENTRY=libp2edrvpptx.la
endif

if HAVE_LIBMAGICK
LIBMAGICK_ENTRY=libp2edrvmagick++.la
endif

if HAVE_LIBEMF
LIBEMF_ENTRY=libp2edrvwmf.la
endif

AM_CPPFLAGS=					\
	${LIBPLOTTER_CFLAGS}			\
	${LIBMING_CFLAGS}			\
	${MKSTEMP_CFLAGS}			\
	${LIBMAGICK_CFLAGS}			\
	${LIBEMF_CFLAGS}			\
	${LIBPNG_CFLAGS}			\
	${LIBZIP_CFLAGS}			\
        -DDEFAULTGS=$(GS)			\
	-DPSTOEDITDATADIR="\"$(pkgdatadir)\""	\
	-DPSTOEDITDOCDIR="\"$(docdir)\""	\
	-DPSTOEDITLIBDIR="\"$(pkglibdir)\""	\
        ${SPECIAL_CXXFLAGS}

lib_LTLIBRARIES=libpstoedit.la
bin_PROGRAMS=pstoedit
pkglib_LTLIBRARIES=libp2edrvstd.la		\
	${LIBPLUGINS_ENTRY}			\
	${LIBPLOTTER_ENTRY}			\
	${LIBMING_ENTRY}			\
	${LIBPPTX_ENTRY}			\
	${LIBMAGICK_ENTRY}			\
	${LIBEMF_ENTRY}

OLD_pkglibdir = $(libdir)/$(PACKAGE)$(PACKAGE_VERSION)

pkglibdir = $(libdir)/$(PACKAGE)


sample_drivers_src=drvsampl.cpp drvsampl.h
specific_drivers_src=				\
	drvgnplt.cpp drvgnplt.h			\
	drvtgif.cpp drvtgif.h			\
	drvfig.cpp drvfig.h			\
	drvidraw.cpp  drvidraw.h

libp2edrvstd_la_SOURCES=			\
	initlibrary.cpp 			\
	drvgcode.cpp    drvgcode.h		\
	drvcairo.cpp    drvcairo.h		\
	drvlwo.cpp      drvlwo.h		\
	drvrib.cpp      drvrib.h		\
	drvrpl.cpp      drvrpl.h		\
	drvdxf.cpp      drvdxf.h 	        \
	dxfacad.h 	dxficad.h		\
	drvjava.cpp 	drvjava.h		\
	drvjava2.cpp 	drvjava2.h		\
	drvpdf.cpp 	drvpdf.h		\
	drvkillu.cpp 	drvkillu.h		\
	drvtext.cpp 	drvtext.h		\
	drvsk.cpp 	drvsk.h			\
	drvmpost.cpp 	drvmpost.h		\
	drvasy.cpp 	drvasy.h		\
	drvmma.cpp 	drvmma.h		\
	drvlatex2e.cpp 	drvlatex2e.h		\
	drvnoi.cpp 	drvnoi.h		\
	drvpic.cpp 	drvpic.h		\
	drvhpgl.cpp 	drvhpgl.h		\
	drvpcb1.cpp	drvpcb1.h               \
	drvpcb2.cpp	drvpcb2.h               \
	drvpcbfill.cpp	drvpcbfill.h            \
	drvpcbrnd.cpp	drvpcbrnd.h libminuid.inc \
	drvgschem.cpp	drvgschem.h             \
	drvcfdg.cpp     drvcfdg.h               \
	drvtk.cpp	drvtk.h                 \
	drvvtk.cpp	drvvtk.h                \
	drvsvm.cpp	drvsvm.h svm_actions.h	\
	$(specific_drivers_src)                 \
        $(sample_drivers_src)                   
libp2edrvstd_la_LIBADD=libpstoedit.la
libp2edrvstd_la_LDFLAGS=-no-undefined -module -avoid-version

#libp2edrvplugins_la_SOURCES=drvmif.cpp drvcgm.cpp cdlib.cpp drvrtf.cpp drvwwmf.cpp
# do not run make dist with these enabled
#libp2edrvplugins_la_SOURCES=drvmif.cpp drvcgm.cpp cdlib.cpp drvrtf.cpp 
#libp2edrvplugins_la_LIBADD=libpstoedit.la
#libp2edrvplugins_la_LDFLAGS=-no-undefined -module -avoid-version
#libp2edrvplugins_la_CPPFLAGS=-I../check/ -I../othersrc/cdlib

libp2edrvlplot_la_SOURCES=drvlplot.cpp drvlplot.h initlibrary.cpp
libp2edrvlplot_la_LIBADD=$(LIBPLOTTER_LDFLAGS) libpstoedit.la
libp2edrvlplot_la_LDFLAGS=-no-undefined -module -avoid-version

libp2edrvpptx_la_SOURCES=drvpptx.cpp drvpptx.h initlibrary.cpp
libp2edrvpptx_la_LIBADD=$(LIBZIP_LDFLAGS) libpstoedit.la
libp2edrvpptx_la_LDFLAGS=-no-undefined -module -avoid-version

libp2edrvswf_la_SOURCES=drvswf.cpp drvswf.h initlibrary.cpp
libp2edrvswf_la_LIBADD=$(LIBMING_LDFLAGS) libpstoedit.la
libp2edrvswf_la_LDFLAGS=-no-undefined -module -avoid-version

# note the __ stand for ++ - automake treats all no alphanums chars as _
libp2edrvmagick___la_SOURCES=drvmagick++.cpp drvmagick++.h initlibrary.cpp
libp2edrvmagick___la_LIBADD=${LIBMAGICK_LDFLAGS} libpstoedit.la
libp2edrvmagick___la_LDFLAGS=-no-undefined -module -avoid-version
# moved to configure.ac because needed only for g++ libp2edrvmagick___la_CPPFLAGS=$(LIBMAGICK_CFLAGS) $(AM_CPPFLAGS) -Wno-long-long

if USE_EMFBYSOURCE
libp2edrvwmf_la_SOURCES=emflib.cpp drvwmf.cpp drvwmf.h initlibrary.cpp
libp2edrvwmf_la_LIBADD=libpstoedit.la 
libp2edrvwmf_la_LDFLAGS=-no-undefined -module -avoid-version
else
libp2edrvwmf_la_SOURCES=drvwmf.cpp drvwmf.h initlibrary.cpp
libp2edrvwmf_la_LIBADD=$(LIBEMF_LDFLAGS) libpstoedit.la
libp2edrvwmf_la_LDFLAGS=-no-undefined -module -avoid-version
endif

libpstoedit_la_SOURCES =			\
	dynload.cpp dynload.h			\
	callgs.cpp				\
	fillpoly.cpp				\
	lexyy.cpp				\
	miscutil.cpp 				\
	cbstream.cpp cbstream.h			\
	psimage.cpp				\
	drvbase.cpp				\
	base64writer.cpp			\
	poptions.cpp				\
	subpath.cpp  subpath.h			\
	pstoedit.cpp				\
	psfront.h				\
	genericints.h

libpstoedit_la_LIBADD = ${CXX_STD_LIB} ${CXX_RUNTIME_LIB} -ldl -lm
libpstoedit_la_LDFLAGS = -no-undefined

pstoedit_SOURCES = cmdmain.cpp
pstoedit_LDADD = libpstoedit.la ${LIBLD_LDFLAGS}
pstoedit_LDFLAGS = -no-undefined

pkginclude_HEADERS =				\
	pstoedit.h				\
	pstoedll.h				\
	pstoeditoutputlib.h			\
        pstoeditoptions.h			\
	cppcomp.h				\
	drvbase.h				\
	drvfuncs.h				\
	miscutil.h				\
	genericints.h				\
	base64writer.h				\
	poptions.h				\
	papersizes.h				\
	ordlist.h				\
	psimage.h


pstoedit.cpp : pstoedit.ph

pstoedit.ph : pstoedit.pro
	awk -f ps_pro_to_ph.awk < pstoedit.pro > pstoedit.ph

cppcheck:
	cppcheck --inconclusive --enable=all `sh ./get_D_and_I_options.sh ${AM_CPPFLAGS}` ${SOURCES}

clanganalyze:
	for f in `ls ${SOURCES} | grep -v \.h | sort -u` ; do \
	  echo "checking $$f"; \
          clang++ --analyze -Weverything -pedantic `sh ./get_D_and_I_options.sh ${AM_CPPFLAGS}` $$f ; \
        done

clang-tidy_allinone:
	clang-tidy  -checks='*' ${SOURCES} --  -std=c++11 `sh ./get_D_and_I_options.sh ${AM_CPPFLAGS}`

clang-tidy_single:
	for f in `ls ${SOURCES} | grep -v \.h | sort -u` ; do \
          echo "checking $$f"; \
          clang-tidy  -checks='*' $$f  -- -std=c++11 `sh ./get_D_and_I_options.sh ${AM_CPPFLAGS}` ; \
        done
