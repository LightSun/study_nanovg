#include <jni.h>
#include "platform.h"

EXTERN_C_ENTER
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H
EXTERN_C_EXIT

#include "fthelper.h"
#include "path-extractor.h"
#include "log.h"
#include "utfs.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong
Java_com_iamjinge_freetype_PathExtractor_nInit(JNIEnv *env, jclass, jstring filePath)
{
    PathExtractor *pathExtractor = new PathExtractor(env->GetStringUTFChars(filePath, JNI_FALSE));
    if (pathExtractor->isValid())
    {
        return reinterpret_cast<jlong>(pathExtractor);
    } else
    {
        delete pathExtractor;
        return 0;
    }
}

JNIEXPORT void
Java_com_iamjinge_freetype_PathExtractor_nFinalize(JNIEnv *env, jclass, jlong nPointer)
{
    if (nPointer)
    {
        PathExtractor *pathExtractor = reinterpret_cast<PathExtractor *>(nPointer);
        delete pathExtractor;
    }
}

JNIEXPORT void
Java_com_iamjinge_freetype_PathExtractor_nSetTextSize(
        JNIEnv *env, jclass, jlong nPointer, jint textSize)
{
    if (nPointer)
    {
        PathExtractor *pathExtractor = reinterpret_cast<PathExtractor *>(nPointer);
        pathExtractor->setTextSize(textSize);
    }
}

JNIEXPORT void
Java_com_iamjinge_freetype_PathExtractor_nGetMetrics(
        JNIEnv *env, jclass, jlong nPointer, jobject jMetrics)
{
    if (nPointer)
    {
        PathExtractor *pathExtractor = reinterpret_cast<PathExtractor *>(nPointer);
        FT_Size_Metrics metrics = pathExtractor->getMetrics();
        jclass metricsClass = env->GetObjectClass(jMetrics);
        env->SetFloatField(jMetrics, env->GetFieldID(metricsClass, "height", "F"), (float)metrics.height);
        env->SetFloatField(jMetrics, env->GetFieldID(metricsClass, "ascender", "F"),
                           (float)metrics.ascender);
        env->SetFloatField(jMetrics, env->GetFieldID(metricsClass, "descender", "F"),
                           (float)metrics.descender);
    }
}

JNIEXPORT jboolean JNICALL
Java_com_iamjinge_freetype_PathExtractor_nExtractPath(
        JNIEnv *env, jclass, jlong nPointer,
        jchar content, jobject path, jintArray boxArray)
{
    if (!nPointer)
    {
        return JNI_FALSE;
    }
    PathExtractor *pathExtractor = reinterpret_cast<PathExtractor *>(nPointer);
    wchar_t wChar = content;

    JPath jPath;
    jPath.env = env;
    jPath.path = path;

    FT_BBox box;

    if (pathExtractor->extractPath(wChar, &jPath, &box))
    {
        LOGD("find %ld, %ld, %ld, %ld", box.xMin, box.xMax, box.yMin, box.yMax);
        env->SetIntArrayRegion(boxArray, 0, 4, reinterpret_cast<const jint *>(&box));
        return JNI_TRUE;
    } else
    {
        LOGD("not find");
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_iamjinge_freetype_PathExtractor_nExtractChPath(
        JNIEnv *env, jclass, jlong nPointer,
        jint content, jobject path, jintArray boxArray)
{
    if (!nPointer)
    {
        return JNI_FALSE;
    }
    PathExtractor *pathExtractor = reinterpret_cast<PathExtractor *>(nPointer);

    JPath jPath;
    jPath.env = env;
    jPath.path = path;

    FT_BBox box;

    if (pathExtractor->extractPath((unsigned long)content, &jPath, &box))
    {
        LOGD("nExtractChPath >>> find %ld, %ld, %ld, %ld", box.xMin, box.xMax, box.yMin, box.yMax);
        env->SetIntArrayRegion(boxArray, 0, 4, reinterpret_cast<const jint *>(&box));
        return JNI_TRUE;
    } else
    {
        LOGD("nExtractChPath >>> not find");
        return JNI_FALSE;
    }
}


#ifdef __cplusplus
}
#endif
extern "C"
JNIEXPORT jint JNICALL
Java_com_iamjinge_freetype_PathExtractor_toUcs2(JNIEnv *env, jclass clazz, jchar ch) {
    // TODO: implement toUcs2()
    unsigned short ucs[3];
    unsigned char ch3[4];
    memcpy(ch3, &ch, 3);
    ch3[3] = '\0';

    auto len = Utf8ToUcs2(ch3, 3, ucs, 2);
    LOGD("len = %d",len);
    return ucs[0];
}