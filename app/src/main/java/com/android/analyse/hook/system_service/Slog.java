package com.android.analyse.hook.system_service;

import com.common.log;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XC_MethodReplacement;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;

public class Slog {
    static void dolog(XC_MethodHook.MethodHookParam param) {
        String logs = "";
        for (Object item : param.args) {
            logs += " " + item + ", ";
        }
        log.i("Slog:" + logs);
//        if (logs.contains("reecess") || logs.contains("expert.hd")) {
//        }
    }

    static public void Hook(ClassLoader classLoader) {
        XposedBridge.hookAllMethods(XposedHelpers.findClass("android.util.Slog", classLoader),
                "d", new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        dolog(param);
                    }
                });
        XposedBridge.hookAllMethods(XposedHelpers.findClass("android.util.Slog", classLoader),
                "e", new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        dolog(param);
                    }
                });
    }
}
