cmake_minimum_required(VERSION 3.21)

project(animitta LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

include(GNUInstallDirs)

# Build in build/<BuildType>/cmake, place final runtime binaries in build/<BuildType>/bin.
get_filename_component(ANIMITTA_BUILD_ROOT "${CMAKE_BINARY_DIR}" DIRECTORY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ANIMITTA_BUILD_ROOT}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ANIMITTA_BUILD_ROOT}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ANIMITTA_BUILD_ROOT}/${CMAKE_INSTALL_LIBDIR}")

find_package(sdl QUIET CONFIG)
if(NOT sdl_FOUND)
  find_package(SDL2 REQUIRED CONFIG)
endif()

find_package(sdl_ttf QUIET CONFIG)
if(NOT sdl_ttf_FOUND)
  find_package(SDL2_ttf REQUIRED CONFIG)
endif()

find_package(OpenGL REQUIRED)

if(UNIX AND NOT APPLE)
  find_library(LIBUTIL util)
endif()

function(animitta_apply_warnings target_name)
  target_compile_options("${target_name}" PRIVATE -Wall -Wextra)
endfunction()

function(animitta_link_sdl target_name)
  if(TARGET sdl::sdl)
    target_link_libraries("${target_name}" PRIVATE sdl::sdl)
  elseif(TARGET SDL2::SDL2)
    target_link_libraries("${target_name}" PRIVATE SDL2::SDL2)
  elseif(TARGET SDL2::SDL2-static)
    target_link_libraries("${target_name}" PRIVATE SDL2::SDL2-static)
  else()
    message(FATAL_ERROR "SDL target not found (expected sdl::sdl or SDL2::SDL2)")
  endif()
endfunction()

function(animitta_link_sdl_ttf target_name)
  if(TARGET sdl_ttf::sdl_ttf)
    target_link_libraries("${target_name}" PRIVATE sdl_ttf::sdl_ttf)
  elseif(TARGET SDL2_ttf::SDL2_ttf)
    target_link_libraries("${target_name}" PRIVATE SDL2_ttf::SDL2_ttf)
  elseif(TARGET SDL2_ttf::SDL2_ttf-static)
    target_link_libraries("${target_name}" PRIVATE SDL2_ttf::SDL2_ttf-static)
  else()
    message(FATAL_ERROR "SDL_ttf target not found")
  endif()
endfunction()

function(animitta_link_pty target_name)
  if(LIBUTIL)
    target_link_libraries("${target_name}" PRIVATE "${LIBUTIL}")
  endif()
endfunction()

add_executable(cuterm cuterm.c)
animitta_apply_warnings(cuterm)
animitta_link_sdl(cuterm)
animitta_link_sdl_ttf(cuterm)
target_link_libraries(cuterm PRIVATE OpenGL::GL)
animitta_link_pty(cuterm)
install(TARGETS cuterm RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(sdl_demo sdl_demo.c)
animitta_apply_warnings(sdl_demo)
animitta_link_sdl(sdl_demo)
install(TARGETS sdl_demo RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(ppm_demo ppm_demo.c)
animitta_apply_warnings(ppm_demo)
install(TARGETS ppm_demo RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

add_executable(pty_smoke pty_smoke.c)
animitta_apply_warnings(pty_smoke)
animitta_link_pty(pty_smoke)
install(TARGETS pty_smoke RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
