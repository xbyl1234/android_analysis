package com.android.analyse.hook.system_service;

import android.content.Intent;
import android.os.Bundle;

import com.common.log;
import com.frida.frida_helper;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class ReceiverDispatcher {

    static public void Hook(ClassLoader classLoader) {
//        XposedHelpers.findAndHookMethod("android.app.LoadedApk$ReceiverDispatcher$InnerReceiver", classLoader, "performReceive",
//                Intent.class, int.class, String.class, Bundle.class, boolean.class, boolean.class, boolean.class, int.class, int.class, String.class,
//                new XC_MethodHook() {
//                    @Override
//                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
//                        super.beforeHookedMethod(param);
//                        log.i("performReceive: " +
//                                frida_helper.object_2_string(param.args[0]) + ", " +
//                                param.args[0] + ", " +
//                                param.args[1] + ", " +
//                                param.args[2] + ", " +
//                                param.args[3] + ", " +
//                                param.args[4] + ", " +
//                                param.args[5] + ", " +
//                                param.args[6] + ", " +
//                                param.args[7] + ", " +
//                                param.args[8] + ", " +
//                                param.args[9] + ", "
//                        );
//                    }
//                });

        XposedHelpers.findAndHookMethod("android.app.LoadedApk$ReceiverDispatcher$InnerReceiver", classLoader, "performReceive",
                Intent.class, int.class, String.class, Bundle.class, boolean.class, boolean.class, int.class,
                new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        log.i("performReceive: " +
                                frida_helper.object_2_string(param.args[0]) + ", " +
                                param.args[1] + ", " +
                                param.args[2] + ", " +
                                frida_helper.object_2_string(param.args[3]) + ", " +
                                param.args[4] + ", " +
                                param.args[5] + ", " +
                                param.args[6]
                        );
                    }
                });


    }
}
