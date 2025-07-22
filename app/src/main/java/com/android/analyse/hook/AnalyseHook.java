package com.android.analyse.hook;


import android.app.Application;

import com.android.analyse.hook.system_service.SettingsProvider;
import com.common.log;
import com.common.tools.hooker.WhenHook;

import java.io.File;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class AnalyseHook implements IXposedHookLoadPackage {
    static boolean hadHook = false;
    static int idx = 0;
    void findJniNative(ClassLoader classLoader) {
        try {
            Native.find_jni_native_addr(new Class[]{XposedHelpers.findClass("m41739d0d", classLoader)});
        } catch (Throwable e) {
            log.e("findJniNative " + e);
        }
    }
    void InjectApp(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        try {
            new File("/sdcard/Android/data/" + lpparam.packageName).mkdir();
        } catch (Throwable e) {
            log.i("make extern dir error: " + e);
        }
        FridaHelperLoader.InjectFridaHelp(lpparam.classLoader);
        Native.LoadAnalyseSo(lpparam.packageName);
//        JniTrace.InitJniTrace(FridaHelperLoader.FridaHelper);
        WhenHook.WhenAttach.When(new WhenHook.WhenHookAttachCallback() {
            @Override
            public void OnHookBefore(Application application) {
                try {
//                    application.getExternalCacheDir();
                    //ApplicationPackageManager.Hook(classLoader);
                    //IActivityTaskManagerStubProxy.Hook(classLoader);
                } catch (Throwable e) {
                    log.e("hook context error: " + e);
                }
            }
            @Override
            public void OnHookAfter(Application application) {
            }
        });

//        ActivityTaskManagerService.Hook(lpparam);
//        HookLog.Init(lpparam);
//        DexDump.HookDump2Dex(lpparam, "/data/data/com.global.frxx");
    }
    void InjectSystem(XC_LoadPackage.LoadPackageParam lpparam) {
        WhenHook.WhenDexLoaded.When(new WhenHook.WhenDexLoadedCallback() {
            @Override
            public void OnHook(ClassLoader classLoader, Object[] params) {
                try {
                    log.i("WhenDexLoaded hook setting");
                    SettingsProvider.Hook(classLoader);
                    log.i("hook setting");
                } catch (Throwable e) {
                    log.e("hook setting error: " + e);
                }
            }
        });
        try {
            SettingsProvider.Hook(lpparam.classLoader);
            log.i("hook setting");
        } catch (Throwable e) {
            log.e("hook setting error: " + e);
        }
    }
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        if (lpparam.packageName != null &&
                (lpparam.packageName.contains("com.android.analyse") ||
                        lpparam.packageName.contains("com.android.webview") ||
                        lpparam.packageName.contains("com.google.android.webview"))) {
            return;
        }
        try {
            log.i("analyse inject process: " + lpparam.packageName + ". " + lpparam.processName);
            if (lpparam.packageName.equals("com.android.providers.settings") || lpparam.packageName.equals("android")) {
                InjectSystem(lpparam);
            } else {
                if (lpparam.processName != null) {
                    if (!hadHook) {
                        InjectApp(lpparam);
                    }
                }
            }
            log.i("analyse inject process finish");
        } catch (Throwable e) {
            log.e("analyse inject error: " + e + ", " + Log.getStackTraceString(e));
        }
    }
}
