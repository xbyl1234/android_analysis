package com.android.analyse;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FileManger.CpyFile(this);
//        Test.Test();
//        System.loadLibrary("analyse");
//        Native.nativeInitJniTrace(frida_helper.class);
//        String dataPaht = "/data/data/" + getApplicationInfo().packageName;
//        nativeDumpSo("libc.so", dataPaht);
//        nativeInitHook();
//        nativeEnumSymbols();
//        System.loadLibrary("test");
    }
}