package com.android.analyse.hook.system_service;

import android.os.Message;

import com.common.log;
import com.frida.frida_helper;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class FreecessHandler {

    static public void Hook(ClassLoader classLoader) {
        XposedHelpers.findAndHookMethod("com.android.server.am.FreecessHandler$MainHandler", classLoader, "handleMessage",
                Message.class, new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        log.i("FreecessHandler: " + frida_helper.object_2_string(param.args[0]));
                    }
                });
    }

}
