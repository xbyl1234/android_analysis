package com.android.analyse.hook.code_location;

import android.util.Log;

import com.common.log;

import java.util.HashMap;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class HookData {
    static String[] watchKey = {
            "meta",
            "payload",
            "placementMeta",
            "requestSignal"
    };

    static public boolean checkKey(Object obj) {
        for (String item : watchKey) {
            if (obj != null && obj.toString().contains(item)) {
                log.i("find key: " + obj + ", " + Log.getStackTraceString(new Throwable()));
                return true;
            }
        }
        return false;
    }

    static public void Hook(ClassLoader classLoader) {
        XposedHelpers.findAndHookMethod(HashMap.class, "put", Object.class, Object.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
                checkKey(param.args[0]);
            }
        });
    }

}
