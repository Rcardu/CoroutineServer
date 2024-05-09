cmake_minimum_required(VERSION 3.15)
project(Coroutine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
  set(_WIN32_WINNT 0x601)
  add_definitions(-D_WIN32_WINNT=0x0601)
  add_compile_options(
    $<$<CONFIG:Debug>:/MTd> # ---|-- Statically link the runtime libraries
    $<$<CONFIG:Release>:/MT> # --|
  )
endif()

set(HAPTH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LPATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(EPATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(Boost_ROOT "D:/software/VsCode/Boost")
find_package(Boost 1.84.0 REQUIRED)

set(jsoncpp_ROOT "D:/software/VsCode/Cppsoft/jsoncpp-1.9.5/destage")
set(jsoncpp_INCLUDE_DIR ${jsoncpp_ROOT}/include)
set(jsoncpp_LIBRARY_DIR ${jsoncpp_ROOT}/lib)

set(COROLIB Coroutine)

add_subdirectory(src)
add_subdirectory(test)
