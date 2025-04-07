package com.android.analyse.hook.system_service;

import android.content.pm.PackageManager;
import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import java.util.List;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.pm.PackageManagerService")
public class PackageManagerService extends FakeClassBase {
    static PackageManagerService self;

    static public void Hook(ClassLoader classLoader) {
        self = new PackageManagerService();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    public void setEnabledSettings(XC_MethodHook.MethodHookParam params,
                                   List<PackageManager.ComponentEnabledSetting> settings, int userId, String callingPackage) throws Throwable {
        String logs = frida_helper.object_2_string(settings);
        if (logs.contains("com.expert.hd.wallpaper")) {
            log.i("setEnabledSettings: " + frida_helper.object_2_string(settings) + ", userId:" + userId + ", callingPackage:" + callingPackage + ", " + Log.getStackTraceString(new Exception()));
        }
        CallOriginalMethod(params);
    }
}
