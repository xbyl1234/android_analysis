package com.android.analyse.hook.system_service;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.UserHandle;

import com.common.log;
import com.frida.frida_helper;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;

public class ContextWrapper {

    static public void logRegisterReceiver(XC_MethodHook.MethodHookParam param) {
        String logs = "";
        for (Object item : param.args) {
            if (item != null && item.getClass() == IntentFilter.class) {
                IntentFilter intentFilter = ((IntentFilter) item);
                logs += frida_helper.object_2_string(intentFilter) + ", ";
            } else {
                logs += item + ", ";
            }
        }
        log.i("registerReceiver: " + logs);
    }

    static public void Hook(ClassLoader classLoader) {
//        XposedBridge.hookAllMethods(XposedHelpers.findClass("android.content.ContextWrapper", classLoader),
//                "bindService", new XC_MethodHook() {
//                    @Override
//                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
//                        super.beforeHookedMethod(param);
//                        log.i("bindService " + param.args[0] + ", " + param.args[1].getClass().getName());
//                    }
//                });

        XposedBridge.hookAllMethods(XposedHelpers.findClass("android.content.ContextWrapper", classLoader),
                "registerReceiver", new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        logRegisterReceiver(param);
                    }
                });
        XposedBridge.hookAllMethods(XposedHelpers.findClass("android.content.ContextWrapper", classLoader),
                "registerReceiverForAllUsers", new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        logRegisterReceiver(param);
                    }
                });
        XposedBridge.hookAllMethods(XposedHelpers.findClass("android.content.ContextWrapper", classLoader),
                "registerReceiverAsUser", new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        logRegisterReceiver(param);
                    }
                });

    }
}
