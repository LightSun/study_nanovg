cmake_minimum_required(VERSION 3.4.1)

include_directories(src/main/cpp/freetype/include)
include_directories(src/main/cpp/freetype2/include)

aux_source_directory(src/main/cpp SRC_nanovg)

add_library(
        nanovg
        SHARED
        src/main/cpp/nanovg.c

        src/main/cpp/freetype/src/autofit/autofit.c
        src/main/cpp/freetype/src/base/ftbase.c
        src/main/cpp/freetype/src/base/ftbbox.c
        src/main/cpp/freetype/src/base/ftbitmap.c
        src/main/cpp/freetype/src/base/ftdebug.c
        src/main/cpp/freetype/src/base/ftfstype.c
        src/main/cpp/freetype/src/base/ftglyph.c
        src/main/cpp/freetype/src/base/ftinit.c
        src/main/cpp/freetype/src/base/ftpatent.c
        src/main/cpp/freetype/src/base/ftstroke.c

        src/main/cpp/freetype/src/base/ftsynth.c
        src/main/cpp/freetype/src/base/ftsystem.c
        src/main/cpp/freetype/src/base/fttype1.c
        src/main/cpp/freetype/src/base/ftwinfnt.c

        src/main/cpp/freetype/src/cff/cff.c
        src/main/cpp/freetype/src/cid/type1cid.c
        src/main/cpp/freetype/src/gzip/ftgzip.c
        src/main/cpp/freetype/src/psaux/psaux.c
        src/main/cpp/freetype/src/pshinter/pshinter.c
        src/main/cpp/freetype/src/psnames/psnames.c
        src/main/cpp/freetype/src/raster/raster.c

        src/main/cpp/freetype/src/sfnt/sfnt.c
        src/main/cpp/freetype/src/smooth/smooth.c
        src/main/cpp/freetype/src/truetype/truetype.c
        src/main/cpp/freetype/src/type1/type1.c
)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)


target_link_libraries(
        nanovg
        ${log-lib}
)