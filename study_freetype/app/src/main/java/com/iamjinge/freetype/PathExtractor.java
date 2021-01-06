package com.iamjinge.freetype;

import android.graphics.Matrix;
import android.graphics.Path;
import android.graphics.RectF;
import android.util.Log;

import androidx.annotation.Keep;

/**
 * setlocal, 文件编码, freetype编码, ttf是否支持.
 */
public class PathExtractor {
    private static final String TAG = "PathExtractor";

    /**
     * https://www.freetype.org/freetype2/docs/glyphs/glyphs-6.html
     * An outline point's vectorial coordinates are expressed in the 26.6 format, i.e.,
     * in 1/64th of a pixel, hence the coordinates ‘(1.0,-2.5)’ is stored as
     * the integer pair ‘(64,-192)’, to name an example.
     */
    private static final float SIZE_MULTI = 1 / 64f;

    private String filePath;

    private long nativePointer;

    public PathExtractor(String filePath) {
        this.filePath = filePath;
        nativePointer = nInit(filePath);
        if (nativePointer != 0) {
            Log.i(TAG, "create for " + filePath + " " + nativePointer);
        } else {
            Log.e(TAG, "create for " + filePath + " failed");
        }
    }

    public boolean isValid() {
        return nativePointer != 0;
    }

    public void setTextSize(int textSize) {
        Log.i(TAG, "set text size " + nativePointer + textSize);
        nSetTextSize(nativePointer, textSize);
    }

    public void getMetrics(Metrics metrics) {
        if (metrics == null) {
            Log.w(TAG, "metrics is null");
            return;
        }
        nGetMetrics(nativePointer, metrics);
        metrics.height = metrics.height * SIZE_MULTI;
        metrics.ascender = metrics.ascender * SIZE_MULTI;
        metrics.descender = metrics.descender * SIZE_MULTI;
    }

    public boolean extractPath(char content, Path path, RectF boxF) {
        if (path == null) {
            Log.d(TAG, "path is null");
            return false;
        }
        int[] box = new int[4];
        boolean exist = false;
        try {
            if(isChinese(content)){
                exist = nExtractChPath(nativePointer, content, path, box);
            }else {
                exist = nExtractPath(nativePointer, content, path, box);
            }
        } catch (Exception e) {
            Log.e(TAG, "extract path error " + content);
        }
        if (exist) {
            Matrix m = new Matrix();
            m.setScale(SIZE_MULTI, SIZE_MULTI);
            path.transform(m);

            if (boxF != null) {
                boxF.set(box[0] * SIZE_MULTI, box[3] * SIZE_MULTI,
                        box[2] * SIZE_MULTI, box[1] * SIZE_MULTI);
            }
            Log.i(TAG, "load path for " + content);
            return true;
        } else {
            Log.w(TAG, "not such char " + content);
            return false;
        }
    }

    private static boolean isChinese(char content) {
        return String.valueOf(content).matches("[\\u4e00-\\u9fa5]+");
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if(nativePointer != 0){
                nFinalize(nativePointer);
                nativePointer = 0;
            }
        } finally {
            super.finalize();
        }
    }

    @Keep
    public static class Metrics {
        public float height;
        public float ascender;
        public float descender;
    }

    static {
//        System.loadLibrary("freetype");
        System.loadLibrary("study-freetype");
    }

    private static native long nInit(String filePath);

    private static native void nFinalize(long nPointer);

    private static native void nSetTextSize(long nPointer, int textSize);

    private static native void nGetMetrics(long nPointer, Metrics metrics);

    /**
     * @param box left, bottom, right, top
     */
    private static native boolean nExtractPath(long nativePointer, char content, Path path, int[] box);
    private static native boolean nExtractChPath(long nativePointer, int content, Path path, int[] box);

    //todo not work
    private static final native int toUcs2(char ch);

}
