package com.android.analyse.hook.system_service;

import com.common.log;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XC_MethodReplacement;
import de.robv.android.xposed.XposedHelpers;

public class HookSpeechRecognizer {

    static public void Hook(ClassLoader classLoader) {
        XposedHelpers.findAndHookMethod("com.android.analyse.hook.system_service.HookSpeechRecognizer",
                classLoader, "startListening", new XC_MethodReplacement() {
                    @Override
                    protected Object replaceHookedMethod(MethodHookParam param) throws Throwable {
                        log.i("startListening");
                        return null;
                    }
                });

    }
}
