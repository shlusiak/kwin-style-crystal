AUTOMAKE_OPTIONS = foreign

INCLUDES = $(all_includes)

images = vista/*.png \
	aqua/*.png \
	default/*.png \
	handpainted/*.png \
	knifty/*.png \
	svg/*.png

noinst_PROGRAMS = embedtool tiles.h
embedtool_SOURCES = embedtool.cpp
embedtool_LDFLAGS = $(all_libraries)
embedtool_LDADD = ${LIB_QT}
tiles_h_SOURCES = ${images}

tiles.h: embedtool ${images}
	@echo Generating Button Themes...
	./embedtool ${images} > tiles.h
	cp tiles.h ../client/tiles.h
