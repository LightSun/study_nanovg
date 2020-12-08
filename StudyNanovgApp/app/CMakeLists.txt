cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/freetype/include)

set(LIB_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})
add_library(freetype SHARED IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libfreetype.so)

aux_source_directory(src/main/cpp SRC_nanovg)

add_library(
        nanovg
        SHARED
        src/main/cpp/nanovg.c
)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


target_link_libraries(
        nanovg
        freetype
        ${log-lib}
)