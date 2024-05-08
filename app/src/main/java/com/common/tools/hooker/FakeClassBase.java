package com.common.tools.hooker;

import java.util.HashMap;
import java.util.Map;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;

public abstract class FakeClassBase {
    public Map<String, Hooker.MethodReplacement> fakeMethods = new HashMap<>();
    public Class originClass = null;
    public ClassLoader classLoader;

    public abstract boolean ShouldFake(XC_MethodHook.MethodHookParam params);

    public boolean InitFakeClass(ClassLoader classLoader, Class originClass) throws Throwable {
        return true;
    }

    public boolean InitBaseFakeClass(ClassLoader classLoader, Class originClass) throws Throwable {
        this.classLoader = classLoader;
        this.originClass = originClass;
        InitFakeClass(classLoader, originClass);
        return true;
    }

    static public Object CallOriginalMethod(XC_MethodHook.MethodHookParam params) throws Throwable {
        return XposedBridge.invokeOriginalMethod(params.method, params.thisObject, params.args);
    }

    static public Object CallOriginalMethod(XC_MethodHook.MethodHookParam params, Object... args) throws Throwable {
        return XposedBridge.invokeOriginalMethod(params.method, params.thisObject, args);
    }
}
