add_subdirectory( config )


##### Geneates tiles.h from button images. List subfolders of $top/pics/ that contain *.png files you want to include
set (PICS
	default
	aqua
	handpainted
	knifty
	kubuntu-dapper
	kubuntu-edgy
	kubuntu-feisty
	kubuntu-hardy
	svg
	vista
	glass-pill
    )
set (OVERLAYS
	glass_overlay
	lighting_overlay
	steel_overlay
    )
set (LOGOS
	kde
	tux
	gentoo
	kubuntu
	ubuntu
	opensuse
	pclinuxos
    )

set (PICS_SOURCES)
foreach(dir ${PICS})
   set (PICS_SOURCES ${PICS_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/../pics/${dir}/*.png)
endforeach(dir)

add_custom_command (
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tiles.h
   COMMAND embedtool ${PICS_SOURCES} > ${CMAKE_CURRENT_BINARY_DIR}/tiles.h
   DEPENDS ${PICS_SOURCES}
   )

##### Geneates overlays.h from button images. List files in $top/pics that are to be used as overlay
set (OVERLAY_SOURCES)
foreach(file ${OVERLAYS})
   set (OVERLAY_SOURCES ${OVERLAY_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/../pics/${file}.png)
endforeach(file)

add_custom_command (
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/overlays.h
   COMMAND embedtool ${OVERLAY_SOURCES} > ${CMAKE_CURRENT_BINARY_DIR}/overlays.h
   DEPENDS ${OVERLAY_SOURCES}
   )

##### Geneates logos.h from logo images. List files in $top/pics/logos that are to be used as overlay
set (LOGOS_SOURCES)
foreach(file ${LOGOS})
   set (LOGOS_SOURCES ${LOGOS_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/../pics/logos/${file}.png)
endforeach(file)

add_custom_command (
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/logos.h
   COMMAND embedtool ${LOGOS_SOURCES} > ${CMAKE_CURRENT_BINARY_DIR}/logos.h
   DEPENDS ${LOGOS_SOURCES}
   )

##### Target #####
set(kwin3_crystal_PART_SRCS
   buttonimage.cpp
   crystalbutton.cpp
   crystalclient.cpp
   crystalfactory.cpp
   ${CMAKE_CURRENT_BINARY_DIR}/tiles.h
   ${CMAKE_CURRENT_BINARY_DIR}/overlays.h
   ${CMAKE_CURRENT_BINARY_DIR}/logos.h
   )

kde4_add_plugin(kwin3_crystal ${kwin3_crystal_PART_SRCS})
target_link_libraries(kwin3_crystal ${KDE4_KDEUI_LIBS} kdecorations )

install(TARGETS kwin3_crystal  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES crystal.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )

