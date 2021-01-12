//
// Created by Administrator on 2021/1/12 0012.
//

// example2.cpp

// This file demonstrates how to render a coloured glyph with a differently
// coloured outline.
//
// Written Feb. 2009 by Erik M枚ller,
// with slight modifications by Werner Lemberg
//
// Public domain.
//
// Eric uses similar code in real applications; see
//
//   http://www.timetrap.se
//   http://www.emberwind.se
//
// for more.

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <vector>
#include <fstream>
#include <iostream>


#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif


// Define some fixed size types.

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


// Try to figure out what endian this machine is using. Note that the test
// below might fail for cross compilation; additionally, multi-byte
// characters are implementation-defined in C preprocessors.

#if (('1234' >> 24) == '1')
#elif (('4321' >> 24) == '1')
#define BIG_ENDIAN
#else
  #error "Couldn't determine the endianness!"
#endif


// A simple 32-bit pixel.

union Pixel32
{
    Pixel32()
            : integer(0) { }
    Pixel32(uint8 bi, uint8 gi, uint8 ri, uint8 ai = 255)
    {
        b = bi;
        g = gi;
        r = ri;
        a = ai;
    }

    uint32 integer;

    struct
    {
#ifdef BIG_ENDIAN
        uint8 a, r, g, b;
#else // BIG_ENDIAN
        uint8 b, g, r, a;
#endif // BIG_ENDIAN
    };
};


struct Vec2
{
    Vec2() { }
    Vec2(float a, float b)
            : x(a), y(b) { }

    float x, y;
};


struct Rect
{
    Rect() { }
    Rect(float left, float top, float right, float bottom)
            : xmin(left), xmax(right), ymin(top), ymax(bottom) { }

    void Include(const Vec2 &r)
    {
        xmin = MIN(xmin, r.x);
        ymin = MIN(ymin, r.y);
        xmax = MAX(xmax, r.x);
        ymax = MAX(ymax, r.y);
    }

    float Width() const { return xmax - xmin + 1; }
    float Height() const { return ymax - ymin + 1; }

    float xmin, xmax, ymin, ymax;
};


// TGA Header struct to make it simple to dump a TGA to disc.

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TGAHeader
{
    uint8   idLength,           // Length of optional identification sequence.
            paletteType,        // Is a palette present? (1=yes)
            imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
    // 3=grey, +8=rle packed).
    uint16  firstPaletteEntry,  // First palette index, if present.
            numPaletteEntries;  // Number of palette entries, if present.
    uint8   paletteBits;        // Number of bits per palette entry.
    uint16  x,                  // Horiz. pixel coord. of lower left of image.
            y,                  // Vert. pixel coord. of lower left of image.
            width,              // Image width in pixels.
            height;             // Image height in pixels.
    uint8   depth,              // Image color depth (bits per pixel).
            descriptor;         // Image attribute flags.
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif


bool
WriteTGA(const std::string &filename,
         const Pixel32 *pxl,
         uint16 width,
         uint16 height)
{
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (file)
    {
        TGAHeader header;
        memset(&header, 0, sizeof(TGAHeader));
        header.imageType  = 2;
        header.width = width;
        header.height = height;
        header.depth = 32;
        header.descriptor = 0x20;

        file.write((const char *)&header, sizeof(TGAHeader));
        file.write((const char *)pxl, sizeof(Pixel32) * width * height);

        return true;
    }
    return false;
}


// A horizontal pixel span generated by the FreeType renderer.

struct Span
{
    Span() { }
    Span(int _x, int _y, int _width, int _coverage)
            : x(_x), y(_y), width(_width), coverage(_coverage) { }

    int x, y, width, coverage;
};

typedef std::vector<Span> Spans;


// Each time the renderer calls us back we just push another span entry on
// our list.

void
RasterCallback(int y, int count,
               const FT_Span * spans,
               void * user)
{
    Spans *sptr = (Spans *)user;
    for (int i = 0; i < count; ++i)
        sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
}


// Set up the raster parameters and render the outline.

void
RenderSpans(FT_Library &library,
            FT_Outline * const outline,
            Spans *spans)
{
    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = RasterCallback;
    params.user = spans;

    FT_Outline_Render(library, outline, &params);
}


// Render the specified character as a colored glyph with a colored outline
// and dump it to a TGA.

void
WriteGlyphAsTGA(FT_Library &library,
                const std::string &fileName,
                wchar_t ch,
                FT_Face &face,
                int size,
                const Pixel32 &fontCol,
                const Pixel32 outlineCol,
                float outlineWidth)
{
    // Set the size to use.
    if (FT_Set_Char_Size(face, size << 6, size << 6, 90, 90) == 0)
    {
        // Load the glyph we are looking for.
        FT_UInt gindex = FT_Get_Char_Index(face, ch);
        if (FT_Load_Glyph(face, gindex, FT_LOAD_NO_BITMAP) == 0)
        {
            // Need an outline for this to work.
            if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
            {
                // Render the basic glyph to a span list.
                Spans spans;
                RenderSpans(library, &face->glyph->outline, &spans);

                // Next we need the spans for the outline.
                Spans outlineSpans;

                // Set up a stroker.
                FT_Stroker stroker;
                FT_Stroker_New(library, &stroker);
                FT_Stroker_Set(stroker,
                               (int)(outlineWidth * 64),
                               FT_STROKER_LINECAP_ROUND,
                               FT_STROKER_LINEJOIN_ROUND,
                               0);

                FT_Glyph glyph;
                if (FT_Get_Glyph(face->glyph, &glyph) == 0)
                {
                    FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
                    // Again, this needs to be an outline to work.
                    if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
                    {
                        // Render the outline spans to the span list
                        FT_Outline *o =
                                &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
                        RenderSpans(library, o, &outlineSpans);
                    }

                    // Clean up afterwards.
                    FT_Stroker_Done(stroker);
                    FT_Done_Glyph(glyph);

                    // Now we need to put it all together.
                    if (!spans.empty())
                    {
                        // Figure out what the bounding rect is for both the span lists.
                        Rect rect(spans.front().x,
                                  spans.front().y,
                                  spans.front().x,
                                  spans.front().y);
                        for (Spans::iterator s = spans.begin();
                             s != spans.end(); ++s)
                        {
                            rect.Include(Vec2(s->x, s->y));
                            rect.Include(Vec2(s->x + s->width - 1, s->y));
                        }
                        for (Spans::iterator s = outlineSpans.begin();
                             s != outlineSpans.end(); ++s)
                        {
                            rect.Include(Vec2(s->x, s->y));
                            rect.Include(Vec2(s->x + s->width - 1, s->y));
                        }

#if 0
                        // This is unused in this test but you would need this to draw
            // more than one glyph.
            float bearingX = face->glyph->metrics.horiBearingX >> 6;
            float bearingY = face->glyph->metrics.horiBearingY >> 6;
            float advance = face->glyph->advance.x >> 6;
#endif

                        // Get some metrics of our image.
                        int imgWidth = rect.Width(),
                                imgHeight = rect.Height(),
                                imgSize = imgWidth * imgHeight;

                        // Allocate data for our image and clear it out to transparent.
                        Pixel32 *pxl = new Pixel32[imgSize];
                        memset(pxl, 0, sizeof(Pixel32) * imgSize);

                        // Loop over the outline spans and just draw them into the
                        // image.
                        for (Spans::iterator s = outlineSpans.begin();
                             s != outlineSpans.end(); ++s)
                            for (int w = 0; w < s->width; ++w)
                                pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
                                          + s->x - rect.xmin + w)] =
                                        Pixel32(outlineCol.r, outlineCol.g, outlineCol.b,
                                                s->coverage);

                        // Then loop over the regular glyph spans and blend them into
                        // the image.
                        for (Spans::iterator s = spans.begin();
                             s != spans.end(); ++s)
                            for (int w = 0; w < s->width; ++w)
                            {
                                Pixel32 &dst =
                                        pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
                                                  + s->x - rect.xmin + w)];
                                Pixel32 src = Pixel32(fontCol.r, fontCol.g, fontCol.b,
                                                      s->coverage);
                                dst.r = (int)(dst.r + ((src.r - dst.r) * src.a) / 255.0f);
                                dst.g = (int)(dst.g + ((src.g - dst.g) * src.a) / 255.0f);
                                dst.b = (int)(dst.b + ((src.b - dst.b) * src.a) / 255.0f);
                                dst.a = MIN(255, dst.a + src.a);
                            }

                        // Dump the image to disk.
                        WriteTGA(fileName, pxl, imgWidth, imgHeight);

                        delete [] pxl;
                    }
                }
            }
        }
    }
}


extern "C" int
genTGA(const char* fontfile, const char* tga)
{
    // Initialize FreeType.
    FT_Library library;
    FT_Init_FreeType(&library);

    // Open up a font file.
    std::ifstream fontFile(fontfile, std::ios::binary);
    if (fontFile)
    {
        // Read the entire file to a memory buffer.
        fontFile.seekg(0, std::ios::end);
        std::fstream::pos_type fontFileSize = fontFile.tellg();
        fontFile.seekg(0);
        unsigned char *fontBuffer = new unsigned char[fontFileSize];
        fontFile.read((char *)fontBuffer, fontFileSize);

        // Create a face from a memory buffer.  Be sure not to delete the memory
        // buffer until you are done using that font as FreeType will reference
        // it directly.
        FT_Face face;
        FT_New_Memory_Face(library, fontBuffer, fontFileSize, 0, &face);

        // Dump out a single glyph to a tga.
        WriteGlyphAsTGA(library,
                        tga,
                        L'中',
                        face,
                        100,
                        Pixel32(255, 90, 30),
                        Pixel32(255, 255, 255),
                        3.0f);

        // Now that we are done it is safe to delete the memory.
        delete [] fontBuffer;
    }

    // Clean up the library
    FT_Done_FreeType(library);

    return 1;
}

// Local Variables:
// coding: utf-8
// End: