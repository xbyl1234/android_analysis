package com.andriod.analyse.hook;

import android.app.Application;
import android.content.Context;

import com.common.log;
import com.common.utils;
import com.frida.Reflect;
import com.frida.frida_helper;

import java.util.concurrent.ConcurrentHashMap;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class HookLog {

    static boolean HookMap() {
        XposedHelpers.findAndHookMethod(ConcurrentHashMap.class, "put", Object.class, Object.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
                if (param.args[0].getClass() == String.class) {
                    String key = (String) param.args[0];
                    if (key.length() == 2) {
                        log.i("" + param.args[0]);
                        if (key.equals("tc")) {
                            new Throwable().printStackTrace();
                        }
                    }
                }
            }
        });
        return true;
    }

    static int count = 0;

    static boolean HookAQY(ClassLoader classLoader) {

        XposedHelpers.findAndHookMethod(XposedHelpers.findClass("c42.p", classLoader), "a", Context.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
                new Throwable().printStackTrace();
            }
        });

        XposedHelpers.findAndHookMethod(XposedHelpers.findClass("t32.c", classLoader), "d", String.class, String.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
                log.i("put " + param.args[0] + "  " + param.args[1]);

                Object b = Reflect.GetFieldValue(XposedHelpers.findClass("z32.b", classLoader), null, "b");
                log.i("b: " + frida_helper.object_2_string(b));
                if (count < 10) {
                    utils.save_file("/data/data/com.qiyi.video/json",
                            frida_helper.object_2_string(b).getBytes());
                }
                count++;
                new Throwable().printStackTrace();
            }
        });
        return true;
    }

    static boolean Init(XC_LoadPackage.LoadPackageParam lpparam) {
        boolean result = true;
//        result &= HookMap();


        XposedBridge.hookAllMethods(XposedHelpers.findClass("android.app.ActivityThread", lpparam.classLoader),
                "performLaunchActivity",
                new XC_MethodHook() {
                    @Override
                    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                        super.afterHookedMethod(param);
                        Application mInitialApplication = (Application) XposedHelpers.getObjectField(param.thisObject, "mInitialApplication");
                        ClassLoader classLoader = (ClassLoader) XposedHelpers.callMethod(mInitialApplication, "getClassLoader");
                        log.i("find class loader: " + classLoader.toString());
                        HookAQY(classLoader);
                    }
                });

        return result;
    }
}
