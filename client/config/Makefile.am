AUTOMAKE_OPTIONS = foreign

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

noinst_HEADERS = crystalconfig.h

kde_module_LTLIBRARIES = kwin_crystal_config.la
kwin_crystal_config_la_SOURCES = configdialog.ui infodialog.ui crystalconfig.cc crystalconfig.h
kwin_crystal_config_la_LDFLAGS = -module $(all_libraries) $(KDE_PLUGIN)
kwin_crystal_config_la_LIBADD = $(LIB_KDEUI) $(LIB_KIO) $(LIB_QT) $(LIB_KDECORE)
kwin_crystal_config_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin_crystal_config_la_METASOURCES)

crystalconfig.cc: configdialog.h infodialog.h


messages:
	$(XGETTEXT) *.cpp -o $(podir)/kwin_crystal_config.pot
METASOURCES = AUTO
