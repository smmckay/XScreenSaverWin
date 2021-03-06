# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 2.4)

# use new policy
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0054 NEW)
    cmake_policy(SET CMP0003 NEW)
    if (POLICY CMP0092)
        cmake_policy(SET CMP0092 NEW)
    endif()
endif(COMMAND cmake_policy)

# project name and language
project(XScreenSaverWin C)
enable_language(CXX)

# check build type
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# make uppercase string of build type
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

# check build type again
if (NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL)$")
    message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# set output directory (build/)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Win32 or not?
if (NOT WIN32)
    message(FATAL_ERROR "You must use a Win32 compiler")
endif()

# do statically link
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBRARIES OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    set(CMAKE_C_FLAGS "-static -static-libgcc -static-libstdc++")
    set(CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    set(CMAKE_C_FLAGS "-static")
    set(CMAKE_CXX_FLAGS "-static")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # replace "/MD" with "/MT" (building without runtime DLLs)
    set(CompilerFlags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    foreach(CompilerFlags ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlags} "${${CompilerFlags}}")
    endforeach()
endif()

# enable Win32 resource
enable_language(RC)

##############################################################################

find_package(OpenGL)

# add_screensaver macro
macro(add_screensaver)
    set(SOURCES "")
    set(TARGET "")
    foreach(argument ${ARGN})
        if("${TARGET}" STREQUAL "")
            set(TARGET ${argument})
        else()
            list(APPEND SOURCES ${argument})
        endif()
    endforeach()
    add_executable(${TARGET} ${SOURCES})
    set_target_properties(${TARGET} PROPERTIES SUFFIX ".scr")
    target_link_libraries(${TARGET} comctl32 scrnsave)
endmacro()

# add_screenhack_screensaver macro
macro(add_screenhack_screensaver)
    set(SOURCES "")
    set(TARGET "")
    foreach(argument ${ARGN})
        if("${TARGET}" STREQUAL "")
            set(TARGET ${argument})
        else()
            list(APPEND SOURCES ${argument})
        endif()
    endforeach()
    add_executable(${TARGET} ${SOURCES} ../screenhack_res.rc)
    set_target_properties(${TARGET} PROPERTIES SUFFIX ".scr")
    target_link_libraries(${TARGET} comctl32 screenhack scrnsave)
endmacro()

# add_xlockmore_screensaver macro
macro(add_xlockmore_screensaver)
    set(SOURCES "")
    set(TARGET "")
    foreach(argument ${ARGN})
        if("${TARGET}" STREQUAL "")
            set(TARGET ${argument})
        else()
            list(APPEND SOURCES ${argument})
        endif()
    endforeach()
    add_executable(${TARGET} ${SOURCES} ../xlockmore_res.rc)
    set_target_properties(${TARGET} PROPERTIES SUFFIX ".scr")
    target_link_libraries(${TARGET} comctl32 xlockmore scrnsave)
endmacro()

##############################################################################

add_definitions(-DUSE_GL=1)

include_directories(. include gl-parts/include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC slowed down by many warnings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1")
else()
    add_subdirectory(scrnsave)
endif()

add_subdirectory(debughelper)
add_subdirectory(gle)
add_subdirectory(gl-parts)
add_subdirectory(random)
add_subdirectory(xscreensaver)
add_subdirectory(xscreensaver-text)

add_subdirectory(common)
add_subdirectory(xlockmore)
add_subdirectory(screenhack)

add_subdirectory(nongl-savers)
add_subdirectory(gl-savers)

##############################################################################
