AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes) -I$(kde_includes)/kwin

kwindir = $(kde_datadir)/kwin/
kwin_DATA = crystal.desktop

noinst_HEADERS = crystalclient.h crystalbutton.h imageholder.h buttonimage.h tiles.h overlays.h

kde_module_LTLIBRARIES = kwin3_crystal.la
kwin3_crystal_la_SOURCES = crystalclient.cc myrootpixmap.cc crystalbutton.cpp \
	imageholder.cpp buttonimage.cpp tiles.h overlays.h
kwin3_crystal_la_LIBADD = $(kde_libraries)/libkdecorations.la $(LIB_QT) $(LIB_KDEUI) $(LIB_KDECORE)
kwin3_crystal_la_LDFLAGS = -module $(all_libraries) $(KDE_PLUGIN)
kwin3_crystal_la_METASOURCES = AUTO

CLEANFILES = tiles.h overlays.h
DISTCLEANFILES = $(kwin3_crystal_la_METASOURCES)
