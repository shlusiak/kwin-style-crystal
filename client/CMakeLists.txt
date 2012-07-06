
add_subdirectory( config )

########### next target ###############

set(kwin3_crystal_PART_SRCS
   buttonimage.cpp
   crystalbutton.cpp
   crystalclient.cpp
   )


kde4_add_plugin(kwin3_crystal ${kwin3_crystal_PART_SRCS})

target_link_libraries(kwin3_crystal ${KDE4_KDEUI_LIBS} kdecorations )

install(TARGETS kwin3_crystal  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES crystal.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )

