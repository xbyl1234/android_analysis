package com.android.analyse.hook.system_service;

import android.util.Log;

import com.common.log;

import java.util.ArrayList;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XC_MethodReplacement;
import de.robv.android.xposed.XposedHelpers;

public class Other {

    static public void Hook(ClassLoader classLoader) {
//        XposedHelpers.findAndHookMethod("com.android.internal.app.AppLockPolicy", classLoader, "isAppLockedPackage", String.class, new XC_MethodHook() {
//            @Override
//            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
//                super.afterHookedMethod(param);
//                log.i("isAppLockedPackage: " + param.args[0] + ", " + param.getResult() + ", " + Log.getStackTraceString(new Exception()));
//            }
//        });
//        XposedHelpers.findAndHookMethod("com.android.server.am.MARsPolicyManager", classLoader, "setSCPMList", ArrayList.class, new XC_MethodHook() {
//            @Override
//            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
//                super.afterHookedMethod(param);
//                log.i("setSCPMList: " + param.args[0] + ", " + Log.getStackTraceString(new Exception()));
//            }
//        });

        XposedHelpers.findAndHookMethod("com.android.server.am.ContentProviderHelper", classLoader, "checkContentProviderPermission",
                android.content.pm.ProviderInfo.class, int.class, int.class, int.class, boolean.class, java.lang.String.class,
                new XC_MethodReplacement() {
                    @Override
                    protected Object replaceHookedMethod(MethodHookParam param) throws Throwable {
                        return null;
                    }
                });
    }

}
