cmake_minimum_required(VERSION 2.6)
project(KWIN_CRYSTAL)
set(VERSION 2.0.6)

find_package(KDE4 REQUIRED)

include (CPack)
include (MacroLibrary)
include (KDE4Defaults)

# macro_optional_find_package(OpenGL)



########### configure tests ###############

# KWIN_HAVE_COMPOSITING - whether any compositing support is available
#if( X11_Xcomposite_FOUND AND X11_Xdamage_FOUND )
#    set( KWIN_HAVE_COMPOSITING 1 )
#endif( X11_Xcomposite_FOUND AND X11_Xdamage_FOUND )

# KWIN_HAVE_OPENGL_COMPOSITING - whether OpenGL-based compositing support is available
#if( KWIN_HAVE_COMPOSITING AND OPENGL_FOUND )
#    set( KWIN_HAVE_OPENGL_COMPOSITING 1 )
#endif( KWIN_HAVE_COMPOSITING AND OPENGL_FOUND )

# KWIN_HAVE_XRENDER_COMPOSITING - whether XRender-based compositing support is available
#if( KWIN_HAVE_COMPOSITING AND X11_Xrender_FOUND AND X11_Xfixes_FOUND )
#    set( KWIN_HAVE_XRENDER_COMPOSITING 1 )
#endif( KWIN_HAVE_COMPOSITING AND X11_Xrender_FOUND AND X11_Xfixes_FOUND )

# safety
#if( KWIN_HAVE_OPENGL_COMPOSITING OR KWIN_HAVE_XRENDER_COMPOSITING )
    # ok
#else( KWIN_HAVE_OPENGL_COMPOSITING OR KWIN_HAVE_XRENDER_COMPOSITING )
#    set( KWIN_HAVE_COMPOSITING ) # unset
#endif( KWIN_HAVE_OPENGL_COMPOSITING OR KWIN_HAVE_XRENDER_COMPOSITING )

#macro_log_feature(KWIN_HAVE_COMPOSITING "Compositing support" "XComposite and XDamage extensions and OpenGL or XRender with XFixes are required for KWin compositing support" "http://www.x.org/")

#macro_bool_to_01( OPENGL_FOUND KWIN_HAVE_OPENGL )

########### global ###############

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1 -DVERSION=\\"${VERSION}\\")
include_directories (${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/lib
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/effects
    )


add_subdirectory( pics )
add_subdirectory( client )
