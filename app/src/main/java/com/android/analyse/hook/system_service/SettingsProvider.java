package com.android.analyse.hook.system_service;

import android.os.Binder;
import android.os.Bundle;


import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;

import java.util.HashMap;
import java.util.Map;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.providers.settings.SettingsProvider")
public class SettingsProvider extends FakeClassBase {
    static final Map<String, String> fixedKeyMaps = new HashMap<>();

    static {
        fixedKeyMaps.put("development_settings_enabled", "0");
        fixedKeyMaps.put("enabled_accessibility_services", "");
        fixedKeyMaps.put("accessibility_enabled", "0");
        fixedKeyMaps.put("adb_enabled", "0");
        fixedKeyMaps.put("adb_wifi_enabled", "0");
    }

    static public void Hook(ClassLoader classLoader) {
        Hooker.HookClass(classLoader, new SettingsProvider());
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    private Bundle MakeResult(String value) {
        Bundle b = new Bundle(1);
        b.putString("value", value);
        return b;
    }

    @FakeMethod(needXposedParams = true)
    public Bundle call(XC_MethodHook.MethodHookParam params, String method, String name, Bundle args) throws Throwable {
        int uid = Binder.getCallingUid();
        if (uid > 2000) {
            if (method.startsWith("GET_")) {
                if (fixedKeyMaps.containsKey(name)) {
//                    log.i("fake uid: " + Binder.getCallingUid() + ", settings call method: " + method + ", name: " + name);
                    return MakeResult(fixedKeyMaps.get(name));
                }
            }
        }
        return (Bundle) CallOriginalMethod(params, method, name, args);
    }
}
