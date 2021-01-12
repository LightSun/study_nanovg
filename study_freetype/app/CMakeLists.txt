cmake_minimum_required(VERSION 3.4.1)

#如果当前系统版本过低设置拒绝编译
if (${ANDROID_PLATFORM_LEVEL} LESS 11)
  message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 (currently using ${ANDROID_PLATFORM_LEVEL}).")
  return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
  add_definitions("-DDYNAMIC_ES3")
  set(OPENGL_LIB GLESv2)
else ()
  set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 11)

SET(SRC_BASE ${CMAKE_SOURCE_DIR}/src/main/cpp)
include_directories(${SRC_BASE}/freetype2)
include_directories(${SRC_BASE}/halfbuzz)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})


add_library(harfbuzz STATIC IMPORTED)
set_target_properties(harfbuzz PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libharfbuzz.a)
#add_library(harfbuzz_subset STATIC IMPORTED)
#set_target_properties(harfbuzz_subset PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libharfbuzz-subset.a)
add_library(freetype STATIC IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libfreetype.a)

#https://github.com/tangrams/harfbuzz-icu-freetype

aux_source_directory(src/main/cpp/sample SRC_sample)

add_library(
        study-freetype
        SHARED
        ${SRC_sample}
)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries(
        study-freetype
        freetype
        #harfbuzz_subset
        harfbuzz
        ${OPENGL_LIB}
        EGL
        z
        ${log-lib}
)

