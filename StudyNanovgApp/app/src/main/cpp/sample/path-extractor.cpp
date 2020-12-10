//
// Created by eltonjiang on 2018/6/14.
//

#include "path-extractor.h"
#include "fthelper.h"
#include "log.h"


FreeTypeLibrary::FreeTypeLibrary()
{
    error = FT_Init_FreeType(&m_ftLibrary);
    checkFT_Error(error, "init library");
}

FreeTypeLibrary::~FreeTypeLibrary()
{
    FT_Done_FreeType(m_ftLibrary);
}


FreeTypeFace::FreeTypeFace(const FreeTypeLibrary &library, const char *filename)
{
    error = FT_New_Face(library.m_ftLibrary, filename, 0, &m_ftFace);
    checkFT_Error(error, "new face");
    FT_Select_Charmap(m_ftFace, FT_ENCODING_UNICODE);

    //获取该字体文件所支持的编码类型
    FT_CharMap charmap;
    int encoding = 0;
    for(int i = 0; i < m_ftFace->num_charmaps; i++)
    {
        charmap = m_ftFace->charmaps[i];
        if(NULL != charmap)
        {
            encoding = charmap->encoding;

            LOGD("charmap [%d] id: %c %c %c %c\n", encoding,
                   (encoding>>24)&0xff,
                   (encoding>>16)&0xff,
                   (encoding>>8)&0xff,
                   (encoding)&0xff);
        }
    }
}

FreeTypeFace::~FreeTypeFace()
{
    if (!error)
        FT_Done_Face(m_ftFace);
}


PathExtractor::PathExtractor(const char *filename)
        : m_face(m_library, filename)
{

}

bool PathExtractor::isValid()
{
    return m_face.error == 0 && m_library.error == 0;
}


void PathExtractor::setTextSize(int textSize)
{
    FT_Error error;
    error = FT_Set_Pixel_Sizes(m_face.m_ftFace, 0, static_cast<FT_UInt>(textSize));
    checkFT_Error(error, "set text size");
}

FT_Size_Metrics PathExtractor::getMetrics()
{
    return m_face.m_ftFace->size->metrics;
}

bool PathExtractor::extractPath(wchar_t wChar, JPath *jPath, FT_BBox *box)
{
    if (!loadGlyph(wChar)){
        LOGW("loadGlyph failed. for %s", wChar);
        return false;
    }

    if (!checkOutline())return false;

    flipOutline();

    return decomposeOutline(jPath, box);
}
bool PathExtractor::extractPath(unsigned long ch, JPath *jPath, FT_BBox *box)
{
    if (!loadGlyph(ch)){
        LOGW("loadGlyph failed. for %xd", ch);
        return false;
    }

    if (!checkOutline())return false;

    flipOutline();

    return decomposeOutline(jPath, box);
}
bool PathExtractor::loadGlyph(unsigned long ch) {
    FT_Error error;

    FT_UInt glyph_index = FT_Get_Char_Index(m_face.m_ftFace, ch);
    error = FT_Load_Glyph(m_face.m_ftFace, glyph_index, FT_LOAD_DEFAULT);
    checkFT_Error(error, "load glyph");
    return error == 0;
}

bool PathExtractor::loadGlyph(wchar_t wChar)
{
    FT_Error error;

    FT_UInt glyph_index = FT_Get_Char_Index(m_face.m_ftFace, static_cast<FT_ULong>(wChar));
    error = FT_Load_Glyph(m_face.m_ftFace, glyph_index, FT_LOAD_DEFAULT);
    checkFT_Error(error, "load glyph");
    //print range
    error = FT_Render_Glyph(m_face.m_ftFace->glyph,  FT_RENDER_MODE_NORMAL);

    FT_Bitmap& bmp = m_face.m_ftFace->glyph->bitmap;
    int h = bmp.rows;
    int w = bmp.width;

    char tmp[w * h];
    char* a = tmp;
    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            if((bmp.buffer[i * w  + j]) == 0 )
            {
                *(a++) = '0';
            }
            else
            {
                *(a++) = '1';
            }
        }
        LOGD("%s", tmp);
        memset(tmp, 0, w* h * sizeof(char));
        a -= w;
    }
    return error == 0;
}

bool PathExtractor::checkOutline()
{
    FT_Face face = m_face.m_ftFace;
    FT_GlyphSlot slot = face->glyph;
    FT_Outline &outline = slot->outline;

    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
        return false; // Should never happen.  Just an extra check.

    if (outline.n_contours <= 0 || outline.n_points <= 0)
        return false; // Can happen for some font files.

    FT_Error error = FT_Outline_Check(&outline);
    return error == 0;
}

void PathExtractor::flipOutline()
{
    const FT_Fixed multiplier = 65536L;

    FT_Matrix matrix;

    matrix.xx = 1L * multiplier;
    matrix.xy = 0L * multiplier;
    matrix.yx = 0L * multiplier;
    matrix.yy = -1L * multiplier;

    FT_Face face = m_face.m_ftFace;
    FT_GlyphSlot slot = face->glyph;
    FT_Outline &outline = slot->outline;

    FT_Outline_Transform(&outline, &matrix);
}

bool PathExtractor::decomposeOutline(JPath *jPath, FT_BBox *box)
{
    FT_Outline_Funcs callbacks;

    callbacks.move_to = MoveToFunction;
    callbacks.line_to = LineToFunction;
    callbacks.conic_to = ConicToFunction;
    callbacks.cubic_to = CubicToFunction;

    callbacks.shift = 0;
    callbacks.delta = 0;

    FT_Face face = m_face.m_ftFace;
    FT_GlyphSlot slot = face->glyph;
    FT_Outline &outline = slot->outline;

    FT_Error error = FT_Outline_Decompose(&outline, &callbacks, jPath);
    checkFT_Error(error, "decompose outline");
    FT_Outline_Get_BBox(&outline, box);
    return error == 0;
}


int PathExtractor::MoveToFunction(const FT_Vector *to, void *user)
{
    JPath *jPath = static_cast<JPath *>(user);

    FT_Pos x = to->x;
    FT_Pos y = to->y;

    jclass pathClass = jPath->env->GetObjectClass(jPath->path);
    jmethodID method = jPath->env->GetMethodID(pathClass, "moveTo", "(FF)V");
    jPath->env->CallVoidMethod(jPath->path, method, (float) x, (float) y);

    LOGD("MoveToFunction: x = %d, y = %d", x, y);
    return 0;
}

int PathExtractor::LineToFunction(const FT_Vector *to, void *user)
{
    JPath *jPath = static_cast<JPath *>(user);

    FT_Pos x = to->x;
    FT_Pos y = to->y;

    jclass pathClass = jPath->env->GetObjectClass(jPath->path);
    jmethodID method = jPath->env->GetMethodID(pathClass, "lineTo", "(FF)V");
    jPath->env->CallVoidMethod(jPath->path, method, (float) x, (float) y);

    LOGD("LineToFunction: x = %d, y = %d", x, y);
    return 0;
}

int PathExtractor::ConicToFunction(const FT_Vector *control, const FT_Vector *to, void *user)
{
    JPath *jPath = static_cast<JPath *>(user);

    FT_Pos controlX = control->x;
    FT_Pos controlY = control->y;

    FT_Pos x = to->x;
    FT_Pos y = to->y;

    LOGD("ConicToFunction: x = %d, y = %d", x, y);
    jclass pathClass = jPath->env->GetObjectClass(jPath->path);
    jmethodID method = jPath->env->GetMethodID(pathClass, "quadTo", "(FFFF)V");
    jPath->env->CallVoidMethod(jPath->path, method, (float) controlX, (float) controlY,
            (float) x, (float) y);
    return 0;
}

int PathExtractor::CubicToFunction(const FT_Vector *controlOne, const FT_Vector *controlTwo,
                                   const FT_Vector *to, void *user)
{
    JPath *jPath = static_cast<JPath *>(user);

    FT_Pos controlOneX = controlOne->x;
    FT_Pos controlOneY = controlOne->y;

    FT_Pos controlTwoX = controlTwo->x;
    FT_Pos controlTwoY = controlTwo->y;

    FT_Pos x = to->x;
    FT_Pos y = to->y;

    LOGD("CubicToFunction: x = %d, y = %d", x, y);
    jclass pathClass = jPath->env->GetObjectClass(jPath->path);
    jmethodID method = jPath->env->GetMethodID(pathClass, "cubicTo", "(FFFFFF)V");
    jPath->env->CallVoidMethod(jPath->path, method, (float) controlOneX, (float) controlOneY,
            (float) controlTwoX, (float) controlTwoY, (float) x, (float) y);
    return 0;
}
