package com.heaven7.android;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import androidx.annotation.Nullable;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FreetypeRenderView extends GLSurfaceView {

    public FreetypeRenderView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    private class RenderImpl implements Renderer{
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        }
        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {

        }
        @Override
        public void onDrawFrame(GL10 gl) {

        }
    }
}
