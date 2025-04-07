package com.android.analyse.hook;

import com.common.log;

import dalvik.system.DexClassLoader;

public class FridaHelperLoader {
    static public boolean hasInjectFridaHelper = false;
    static public Class GsonJson;
    static public Class FridaHelper;

    static boolean InjectFridaHelp(ClassLoader classLoader) {
        if (hasInjectFridaHelper) {
            return hasInjectFridaHelper;
        }
        try {
            DexClassLoader dexClassLoader = new DexClassLoader("/data/frida_helper.dex",
                    null, null, classLoader);
//            try {
//                FastJson = dexClassLoader.loadClass("com.alibaba.fastjson2.JSON");
//            } catch (Exception e) {
//                log.e("load com.alibaba.fastjson.JSON error: " + e);
//            }

//            if (FastJson == null) {
//                log.e("load FastJson error!");
//                return false;
//            }

            try {
                GsonJson = dexClassLoader.loadClass("com.fucker.gson.Gson");
            } catch (Exception e) {
                log.e("load com.fucker.gson.Gson error: " + e);
            }
            if (GsonJson == null) {
                log.e("load GsonJson error!");
                return false;
            }
            try {
                FridaHelper = dexClassLoader.loadClass("com.frida.frida_helper");
            } catch (Exception e) {
                log.e("load FridaHelper error!");
                return false;
            }
            hasInjectFridaHelper = true;
            log.i("inject jar success!");
            return true;
        } catch (Exception e) {
            log.e("inject jar error!" + e);
            return false;
        }
    }
}