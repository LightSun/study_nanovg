cmake_minimum_required(VERSION 3.4.1)

#now use internal free type.
#include_directories(src/main/cpp/freetype/include)
#set(DIR_HALFBUZZ_FREETYPE F:/study/github/good/harfbuzz-icu-freetype-master)
set(DIR_HALFBUZZ_FREETYPE D:/study/github/THIRDS/harfbuzz-icu-freetype-master)
include_directories(${DIR_HALFBUZZ_FREETYPE}/freetype/include)
include_directories(${DIR_HALFBUZZ_FREETYPE}/harfbuzz/src)

set(LIB_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})
#add_library(freetype SHARED IMPORTED)
#set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libfreetype.so)

aux_source_directory(src/main/cpp/sample SRC_sample)
#aux_source_directory(src/main/cpp/harfbuzz/src SRC_sample)

#https://github.com/tangrams/harfbuzz-icu-freetype
add_subdirectory(${DIR_HALFBUZZ_FREETYPE} harfbuzz_out)

add_library(
        nanovg
        SHARED
        src/main/cpp/nanovg.c
)

add_library(
    study-nanovg
    SHARED
    ${SRC_sample}
)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries(
        study-nanovg
        harfbuzz
        freetype
        #icucommon
        ${log-lib}
)

target_link_libraries(
        nanovg
        harfbuzz
        freetype
        #icucommon
        ${log-lib}
)

