//
// Created by Administrator on 2021/1/12 0012.
//

#ifndef STUDY_FREETYPE_TEXTUREIMAGE_H
#define STUDY_FREETYPE_TEXTUREIMAGE_H

#include "platform.h"
EXTERN_C_ENTER

#ifndef GLubyte
typedef unsigned char GLubyte;
#endif
#ifndef GLuint
typedef unsigned int GLuint;
#endif

typedef struct TextureImage{

    int width;
    int height;
    GLuint bpp;
    GLuint texID;
    GLubyte * imageData;
}TextureImage;

bool LoadTGA(TextureImage *texture, char *filename);

EXTERN_C_EXIT

#endif //STUDY_FREETYPE_TEXTUREIMAGE_H
