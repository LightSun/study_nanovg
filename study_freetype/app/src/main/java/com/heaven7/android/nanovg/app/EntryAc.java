package com.heaven7.android.nanovg.app;

import android.Manifest;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.heaven7.android.FreetypeTests;
import com.heaven7.android.TargaReader;
import com.heaven7.android.freetype.app.R;
import com.heaven7.core.util.PermissionHelper;

public class EntryAc extends AppCompatActivity {

    PermissionHelper mHelper = new PermissionHelper(this);
    ImageView iv;

    static {
        System.loadLibrary("study-freetype");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_test);
        iv = findViewById(R.id.iv);
        requestPermission();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        mHelper.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
    private void requestPermission(){
        mHelper.startRequestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE,
                1, new PermissionHelper.ICallback() {
                    @Override
                    public void onRequestPermissionResult(String s, int i, boolean b) {
                        if(b){
                            System.out.println("requestPermission ok");
                            doTest();
                        }
                    }
                    @Override
                    public boolean handlePermissionHadRefused(String s, int i, Runnable runnable) {
                        return false;
                    }
                });
    }

    private void doTest() {
        String font = Environment.getExternalStorageDirectory() + "/temp/font.ttf";
        String tag =  Environment.getExternalStorageDirectory() + "/temp/test1.tga";
        FreetypeTests.writeTga(font, tag);
        System.out.println("writeTga done");
    }

    public void onClickDisplayTag(View view) {
        final String tag =  Environment.getExternalStorageDirectory() + "/temp/test1.tga";
        new Thread(new Runnable() {
            @Override
            public void run() {
                final Bitmap image = TargaReader.getImage(tag);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        iv.setImageBitmap(image);
                    }
                });
            }
        }).start();
    }
}
