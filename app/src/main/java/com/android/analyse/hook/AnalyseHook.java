package com.android.analyse.hook;


import android.app.Application;

import com.common.log;
import com.common.tools.hooker.WhenHook;

import java.io.File;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class AnalyseHook implements IXposedHookLoadPackage {
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        if (lpparam.processName.contains("com.andriod.analyse")) {
            return;
        }
        log.i("analyse inject process: " + lpparam.processName);
        try {
            new File("/sdcard/Android/data/" + lpparam.packageName).mkdir();
        } catch (Throwable e) {
            log.i("make extern dir error: " + e);
        }
        FridaHelperLoader.InjectFridaHelp(lpparam.classLoader);
        Native.LoadAnalyseSo(lpparam.packageName);
        WhenHook.WhenPerformLaunchActivityHook(new WhenHook.WhenHookCallback() {
            @Override
            public void OnHook(Application application, ClassLoader classLoader) {
                try {
                    application.getExternalCacheDir();
                    //ApplicationPackageManager.Hook(classLoader);
                    //IActivityTaskManagerStubProxy.Hook(classLoader);
                } catch (Throwable e) {
                    throw new RuntimeException(e);
                }
            }
        });

//        ActivityTaskManagerService.Hook(lpparam);
//        HookLog.Init(lpparam);
        JniTrace.InitJniTrace(FridaHelperLoader.FridaHelper);
//        DexDump.HookDump2Dex(lpparam, "/data/data/com.global.frxx");
    }
}
