AUTOMAKE_OPTIONS = foreign

KDE_CXXFLAGS = -DQT_PLUGIN

INCLUDES = $(all_includes)

noinst_HEADERS = crystalconfig.h

kde_module_LTLIBRARIES = kwin_crystalgl_config.la
kwin_crystalgl_config_la_SOURCES = crystalconfig.cc configdialog.ui infodialog.ui
kwin_crystalgl_config_la_LDFLAGS = -module $(all_libraries) $(KDE_PLUGIN)
kwin_crystalgl_config_la_LIBADD = $(LIB_KDEUI)
kwin_crystalgl_config_la_METASOURCES = AUTO

DISTCLEANFILES = $(kwin_crystalgl_config_la_METASOURCES)

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kwin_crystalgl_config.pot
METASOURCES = AUTO
