package com.android.analyse;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.android.analyse.databinding.ActivityMainBinding;

import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;

import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexFile;


public class MainActivity extends AppCompatActivity {
    private ActivityMainBinding binding;

    static void loadDex(ByteBuffer dexData, ClassLoader classLoader) throws Throwable {
        Class Element = Class.forName("dalvik.system.DexPathList$Element");
        Constructor ElementConstructor = Element.getDeclaredConstructor(DexFile.class);
        Constructor dexFileConstructor = DexFile.class.getDeclaredConstructor(ByteBuffer[].class, ClassLoader.class,
                Array.newInstance(Element, 0).getClass());
        Object dexFile = dexFileConstructor.newInstance(new ByteBuffer[]{dexData}, null, null);
        Object dexElement = ElementConstructor.newInstance(dexFile);
        Field pathListField = BaseDexClassLoader.class.getDeclaredField("pathList");
        pathListField.setAccessible(true);
        Object pathList = pathListField.get(classLoader);
        Field dexElementsField = pathList.getClass().getDeclaredField("dexElements");
        dexElementsField.setAccessible(true);
        Object dexElements = dexElementsField.get(pathList);
        Object newElements = Array.newInstance(Element, Array.getLength(dexElements) + 1);
        System.arraycopy(dexElements, 0, newElements, 1, Array.getLength(dexElements));
        Array.set(newElements, 0, dexElement);
        dexElementsField.set(pathList, newElements);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        FileManger.CpyFile(this);
        findViewById(R.id.button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                System.loadLibrary("analyse");
                Test.Test();
            }
        });

    }
}