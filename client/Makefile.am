AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes) -I$(kde_includes)/kwin

kwindir = $(kde_datadir)/kwin/
kwin_DATA = crystal.desktop

noinst_HEADERS = crystalclient.h crystalbutton.h imageholder.h buttonimage.h

kde_module_LTLIBRARIES = kwin3_crystal.la
kwin3_crystal_la_SOURCES = crystalclient.cc myrootpixmap.cc crystalbutton.cpp \
		imageholder.cpp buttonimage.cpp
kwin3_crystal_la_LIBADD = $(kde_libraries)/libkdecorations.la
kwin3_crystal_la_LDFLAGS = -module $(all_libraries) $(KDE_PLUGIN) -lkdeui
kwin3_crystal_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin3_crystal_la_METASOURCES)
