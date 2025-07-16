package com.android.analyse.hook.system_service;

import android.app.Activity;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import com.common.log;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class HookActivity {
    static public void Hook(ClassLoader classLoader) {
        XposedHelpers.findAndHookMethod(Activity.class, "finish", int.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
                log.i("finish " + param.thisObject + ", " + Log.getStackTraceString(new Throwable()));
            }
        });

        XposedHelpers.findAndHookMethod(XposedHelpers.findClass("android.app.ActivityClient", classLoader),
                "finishActivity", IBinder.class, int.class, Intent.class, int.class, new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        log.i("finishActivity " + param.args[2]);
                    }
                });

    }
}
