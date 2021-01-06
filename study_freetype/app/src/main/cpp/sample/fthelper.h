//
// Created by eltonjiang on 2018/6/10.
//

#ifndef FONTDECODE_FTHELPER_H
#define FONTDECODE_FTHELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include <jni.h>

void checkFT_Error(FT_Error error, const char *msg);

#ifdef __cplusplus
}
#endif

#endif //FONTDECODE_FTHELPER_H
