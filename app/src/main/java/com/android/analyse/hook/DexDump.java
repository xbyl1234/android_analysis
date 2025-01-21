package com.android.analyse.hook;


import com.common.tools.hooker.HookTools;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class DexDump {

    static public void HookDump2Dex(XC_LoadPackage.LoadPackageParam lpparam, String path) {

        XposedBridge.hookAllConstructors(HookTools.FindClass("dalvik.system.InMemoryDexClassLoader", lpparam.classLoader), new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);


            }
        });

    }

}
