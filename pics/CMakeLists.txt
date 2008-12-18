########### next target ###############

add_executable(embedtool
   embedtool.cpp
   )

target_link_libraries(embedtool ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )
