


########### next target ###############

set(kwin_crystal_config_PART_SRCS crystalconfig.cpp )


kde4_add_ui_files(kwin_crystal_config_PART_SRCS configdialog.ui infodialog.ui)

kde4_add_plugin(kwin_crystal_config ${kwin_crystal_config_PART_SRCS})



target_link_libraries(kwin_crystal_config  kdeui kio ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS kwin_crystal_config  DESTINATION ${PLUGIN_INSTALL_DIR} )

