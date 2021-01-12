//
// Created by Administrator on 2021/1/12 0012.
//
#include "com_heaven7_android_FreetypeTests.h"
#include "TextureImage.h"

extern "C" int genTGA(const char* fontfile, const char* tga);

extern "C" JNIEXPORT void JNICALL Java_com_heaven7_android_FreetypeTests_writeTga
        (JNIEnv * env, jclass, jstring font, jstring tga){


    auto fontStr = env->GetStringUTFChars(font, nullptr);
    auto tgaStr = env->GetStringUTFChars(tga, nullptr);

    genTGA(fontStr, tgaStr);
    env->ReleaseStringUTFChars(font, fontStr);
    env->ReleaseStringUTFChars(tga, tgaStr);
}