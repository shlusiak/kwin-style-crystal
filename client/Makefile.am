AUTOMAKE_OPTIONS = foreign

SUBDIRS = config

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = -I/usr/include/freetype2 $(all_includes) -I$(kde_includes)/kwin

kwindir = $(kde_datadir)/kwin/
kwin_DATA = crystalgl.desktop

noinst_HEADERS = crystalclient.h crystalbutton.h imageholder.h glfont.h

kde_module_LTLIBRARIES = kwin3_crystalgl.la
kwin3_crystalgl_la_SOURCES = crystalclient.cc myrootpixmap.cc crystalbutton.cpp \
	imageholder.cpp glfont.cpp
kwin3_crystalgl_la_LIBADD = $(kde_libraries)/libkdecorations.la
kwin3_crystalgl_la_LDFLAGS = -module $(all_libraries) $(KDE_PLUGIN) -lkdecore \
	-lkdeui -lGL -lGLU
kwin3_crystalgl_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin3_crystalgl_la_METASOURCES)

AM_CXXFLAGS = -DQT_CLEAN_NAMESPACE
