package com.heaven7.android.nanovg.app;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import com.heaven7.core.util.PermissionHelper;

public class MainActivity extends AppCompatActivity {

    PermissionHelper mHelper = new PermissionHelper(this);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

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
                            setContentView(R.layout.activity_main);
                        }
                    }
                    @Override
                    public boolean handlePermissionHadRefused(String s, int i, Runnable runnable) {
                        return false;
                    }
                });
    }

    public void onClickTest(View view) {
        startActivity(new Intent(this, EntryAc.class));
    }
}
