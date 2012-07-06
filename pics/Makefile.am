AUTOMAKE_OPTIONS = foreign

INCLUDES = $(all_includes)

images = $(srcdir)/vista/*.png \
	$(srcdir)/aqua/*.png \
	$(srcdir)/default/*.png \
	$(srcdir)/handpainted/*.png \
	$(srcdir)/knifty/*.png \
	$(srcdir)/svg/*.png \
	$(srcdir)/kubuntu-dapper/*.png \
	$(srcdir)/kubuntu-edgy/*.png

noinst_PROGRAMS = embedtool tiles.h
embedtool_SOURCES = embedtool.cpp
embedtool_LDFLAGS = $(all_libraries)
embedtool_LDADD = ${LIB_QT}
tiles_h_SOURCES = ${images}

tiles.h: embedtool ${images}
	@echo Generating Button Themes...
	./embedtool ${images} > tiles.h
	cp tiles.h ../client/tiles.h
