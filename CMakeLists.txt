cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(Wayland)

set(egl_sources src/egl.c
                src/wrapper.c)

wayland_add_protocol_client(egl_sources src/eglstreamwrapper.xml eglstreamwrapper)

add_library(EGL SHARED ${egl_sources})
set_property(TARGET EGL PROPERTY VERSION 1.5.0)
set_property(TARGET EGL PROPERTY SOVERSION 1 )
target_link_libraries(EGL dl wayland-client)
install(TARGETS EGL DESTINATION lib)



set(eglstreamwrapper-server_sources src/eglstreamwrapper-server.c)
wayland_add_protocol_server(eglstreamwrapper-server_sources src/eglstreamwrapper.xml eglstreamwrapper)

add_library(eglstreamwrapper-server SHARED ${eglstreamwrapper-server_sources})
set_property(TARGET eglstreamwrapper-server PROPERTY VERSION 1.0.0)
set_property(TARGET eglstreamwrapper-server PROPERTY SOVERSION 1 )
target_link_libraries(eglstreamwrapper-server wayland-server)
install(TARGETS eglstreamwrapper-server DESTINATION lib)
install(FILES src/eglstreamwrapper-server.h DESTINATION include/eglstreamwrapper)

add_library(wayland-egl SHARED src/wayland-egl.c)
set_property(TARGET wayland-egl PROPERTY VERSION 1.0.0)
set_property(TARGET wayland-egl PROPERTY SOVERSION 1 )
install(TARGETS wayland-egl DESTINATION lib)