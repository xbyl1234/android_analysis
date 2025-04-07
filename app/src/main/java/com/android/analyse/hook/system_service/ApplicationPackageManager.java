package com.android.analyse.hook.system_service;

import android.content.ComponentName;
import android.content.pm.PackageManager;
import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;

import java.util.List;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "android.app.ApplicationPackageManager")
public class ApplicationPackageManager extends FakeClassBase {
    static ApplicationPackageManager self;

    static public void Hook(ClassLoader classLoader) {
        self = new ApplicationPackageManager();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    String makeLog(List<PackageManager.ComponentEnabledSetting> componentNames) {
        String log = "";
        for (PackageManager.ComponentEnabledSetting item : componentNames) {
            log += item + ", ";
        }
        log += "\n" + Log.getStackTraceString(new Throwable());
        return log;
    }

    String makeLog(ComponentName componentNames) {
        String log = "" + componentNames;
        log += "\n" + Log.getStackTraceString(new Throwable());
        return log;
    }

    @FakeMethod(needXposedParams = true)
    public void setComponentEnabledSetting(XC_MethodHook.MethodHookParam params, ComponentName componentName, int newState, int flags) throws Throwable {
        log.i("setComponentEnabledSetting: " + makeLog(componentName));
        CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public void setComponentEnabledSettings(XC_MethodHook.MethodHookParam params, List<PackageManager.ComponentEnabledSetting> settings) throws Throwable {
        log.i("setComponentEnabledSetting: " + makeLog(settings));
        CallOriginalMethod(params);
    }
}
