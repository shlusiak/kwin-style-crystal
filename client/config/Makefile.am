AUTOMAKE_OPTIONS = foreign

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

noinst_HEADERS = crystalconfig.h

kde_module_LTLIBRARIES = kwin_crystal_config.la
kwin_crystal_config_la_SOURCES = crystalconfig.cc configdialog.ui infodialog.ui
kwin_crystal_config_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kwin_crystal_config_la_LIBADD = $(LIB_KDEUI)
kwin_crystal_config_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin_crystal_config_la_METASOURCES)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kwin_crystal_config.pot
METASOURCES = AUTO
