#
#   This file is part of Magnum.
#
#   Original authors — credit is appreciated but not required:
#
#       2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019 —
#           Vladimír Vondruš <mosra@centrum.cz>
#       2018 — ShaddyAQN <ShaddyAQN@gmail.com>
#       2018 — Jonathan Hale <squareys@googlemail.com>
#       2018 — Tomáš Skřivan <skrivantomas@seznam.cz>
#       2018 — Natesh Narain <nnaraindev@gmail.com>
#
#   This is free and unencumbered software released into the public domain.
#
#   Anyone is free to copy, modify, publish, use, compile, sell, or distribute
#   this software, either in source code form or as a compiled binary, for any
#   purpose, commercial or non-commercial, and by any means.
#
#   In jurisdictions that recognize copyright laws, the author or authors of
#   this software dedicate any and all copyright interest in the software to
#   the public domain. We make this dedication for the benefit of the public
#   at large and to the detriment of our heirs and successors. We intend this
#   dedication to be an overt act of relinquishment in perpetuity of all
#   present and future rights to this software under copyright law.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

cmake_minimum_required(VERSION 3.4)

project(MagnumImGuiExample CXX)

set(CMAKE_CXX_STANDARD 20)

# Add module path in case this is project root
if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${PROJECT_SOURCE_DIR}/bin/$<CONFIG>>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${PROJECT_SOURCE_DIR}/bin/$<CONFIG>>)

if(WIN32)
    # for Corrade, Magnum
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /wd4312")
    add_definitions(-D_SILENCE_CXX20_CISO646_REMOVED_WARNING)
    # https://github.com/juj/MathGeoLib/blob/master/CommonOptions.cmake
    # Perform extremely aggressive optimization on Release builds:
    # Flags on Visual Studio 2010 and newer:
    # Runtime library: Multi-threaded (/MT) as opposed to default 'Multi-threaded DLL' - static runtime library allows better LTCG inlining opportunities.
    # Optimization: Full Optimization (/Ox)
    # Inline Function Expansion: Any Suitable (/Ob2)
    # Enable Intrinsic Functions: Yes (/Oi)
    # Favor Size Or Speed: Favor fast code (/Ot)
    # Enable Fiber-Safe Optimizations: Yes (/GT)
    # Enable String Pooling: Yes (/GF)
    # Buffer Security Check: No (/GS-)
    # Floating Point Model: Fast (/fp:fast)
    # Enable Floating Point Exceptions: No (/fp:except-)
    # Build with Multiple Processes (/MP)
    # Whole Program Optimization: Yes (/GL)
    set(relFlags "/O2 /Ob3 /Oi /Ot /GT /GF /GS- /MP /GL /arch:AVX2 /openmp:experimental")
    # Omit Frame Pointers: Yes (/Oy)
    set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE} /Oy ${relFlags}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oy ${relFlags}")

    string(REGEX REPLACE "/Ob1" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REGEX REPLACE "/Ob1" "" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/Ob0" "/Ob1" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

    # Don't omit frame pointers, but add Debug database (/Zi).
    set(CMAKE_C_FLAGS_RELWITHDEBINFO     "${CMAKE_C_FLAGS_RELWITHDEBINFO} /Zi ${relFlags}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Zi ${relFlags}")

    # Disable Incremental Linking (/INCREMENTAL:NO) This is incompatible with LTCG, but RelWithDebInfo has this default on.
    # Perform identical COMDAT folding (/OPT:ICF)
    set(relLinkFlags "/LTCG /INCREMENTAL:NO /OPT:ICF /OPT:REF /PDBCOMPRESS")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${relLinkFlags}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${relLinkFlags}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} ${relLinkFlags}")

    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} ${relLinkFlags}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} ${relLinkFlags}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} ${relLinkFlags}")

    # turn off Exceptions (including in STL) https://gitlab.kitware.com/cmake/cmake/-/issues/20610
    string(REGEX REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_definitions(-D_HAS_EXCEPTIONS=0) # for STL

    # https://ledas.com/post/861-fast-debug-in-visual-c/
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast")
endif()

# Corrade
set(CORRADE_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(WITH_TESTSUITE OFF CACHE BOOL "" FORCE)
add_subdirectory(corrade EXCLUDE_FROM_ALL)
# Magnum
set(MAGNUM_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(MAGNUM_BUILD_PLUGINS_STATIC ON CACHE BOOL "" FORCE)
set(TARGET_DESKTOP_GLES ON CACHE BOOL "" FORCE)
set(WITH_GLFWAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)
# Magnum-Integration
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(MAGNUM_WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(magnum-integration EXCLUDE_FROM_ALL)

find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED GL)
find_package(MagnumIntegration REQUIRED ImGui)

if(CORRADE_TARGET_ANDROID)
    find_package(Magnum REQUIRED AndroidApplication)
elseif(CORRADE_TARGET_EMSCRIPTEN)
    find_package(Magnum REQUIRED EmscriptenApplication)
else()
    find_package(Magnum REQUIRED GlfwApplication)
endif()

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)
corrade_add_resource(Resources resources.conf)
add_executable(magnum-imgui ${Resources} src/ImGuiExample.cpp)

target_link_libraries(magnum-imgui PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    MagnumIntegration::ImGui
)

install(TARGETS magnum-imgui DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT magnum-imgui)
