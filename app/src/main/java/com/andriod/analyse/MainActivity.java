package com.andriod.analyse;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.andriod.analyse.hook.Native;
import com.android_helper.Main;
import com.common.log;
import com.frida.frida_helper;

import java.util.HashMap;

import com.common.utils;

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