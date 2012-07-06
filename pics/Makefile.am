AUTOMAKE_OPTIONS = foreign

INCLUDES = $(all_includes)

images = $(srcdir)/vista/*.png \
	$(srcdir)/aqua/*.png \
	$(srcdir)/default/*.png \
	$(srcdir)/handpainted/*.png \
	$(srcdir)/knifty/*.png \
	$(srcdir)/svg/*.png \
	$(srcdir)/kubuntu-dapper/*.png \
	$(srcdir)/kubuntu-edgy/*.png \
	$(srcdir)/kubuntu-feisty/*.png \
	$(srcdir)/kubuntu-hardy/*.png

overlays = $(srcdir)/lighting_overlay.png \
	$(srcdir)/glass_overlay.png \
	$(srcdir)/steel_overlay.png

EXTRA_DIST = $(srcdir)/vista/* \
	    $(srcdir)/aqua/* \
	    $(srcdir)/default/* \
	    $(srcdir)/handpainted/* \
	    $(srcdir)/knifty/* \
	    $(srcdir)/svg/* \
	    $(srcdir)/kubuntu-dapper/* \
	    $(srcdir)/kubuntu-edgy/* \
	    $(srcdir)/kubuntu-feisty/* \
	    $(srcdir)/kubuntu-hardy/*

noinst_PROGRAMS = embedtool tiles.h overlays.h
nodist_DIST = tiles.h overlays.h
embedtool_SOURCES = embedtool.cpp
embedtool_LDFLAGS = $(all_libraries)
embedtool_LDADD = ${LIB_QT}
tiles_h_SOURCES = ${images}
overlays_h_SOURCES = ${overlays}

tiles.h: embedtool ${images}
	@echo Generating Button Themes...
	./embedtool ${images} > tiles.h
	cp tiles.h ../client/tiles.h

overlays.h: embedtool ${overlays}
	@echo Generating overlays...
	./embedtool ${overlays} > overlays.h
	cp overlays.h ../client/overlays.h

all: tiles.h overlays.h
