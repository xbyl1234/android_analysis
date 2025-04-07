package com.android.analyse.hook.system_service;

import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.am.FreecessController")
public class FreecessController extends FakeClassBase {
    static FreecessController self;

    static public void Hook(ClassLoader classLoader) {
        self = new FreecessController();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    public final boolean freezeAction(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "com.android.server.am.FreecessPkgStatus") Object freecessPkgStatus0, String s, int v, boolean z, boolean z1) throws Throwable {
        log.i("freezeAction " + s + ", " + frida_helper.object_2_string(freecessPkgStatus0) + ", " + Log.getStackTraceString(new Exception()));
        boolean result = (boolean) CallOriginalMethod(params);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public void freezeBgPackageLocked(XC_MethodHook.MethodHookParam params, String s, int v) throws Throwable {
        log.i("freezeBgPackageLocked " + s + ", " + Log.getStackTraceString(new Exception()));
        CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public boolean freezeForCalmMode(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "com.android.server.am.FreecessPkgStatus") Object freecessPkgStatus0, String s) throws Throwable {
        log.i("freezeForCalmMode " + s + ", " + frida_helper.object_2_string(freecessPkgStatus0) + ", " + Log.getStackTraceString(new Exception()));
        boolean result = (boolean) CallOriginalMethod(params);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final boolean freezeForOLAF(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "com.android.server.am.FreecessPkgStatus") Object freecessPkgStatus0, String s) throws Throwable {
        log.i("freezeForOLAF " + s + ", " + frida_helper.object_2_string(freecessPkgStatus0) + ", " + Log.getStackTraceString(new Exception()));
        boolean result = (boolean) CallOriginalMethod(params);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public boolean freezePackage(XC_MethodHook.MethodHookParam params, int v, String s, int v1, boolean z, boolean z1) throws Throwable {
        log.i("freezePackage2 " + s + ", " + Log.getStackTraceString(new Exception()));
        boolean result = (boolean) CallOriginalMethod(params);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final void freezeOLAFPackage(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "com.android.server.am.FreecessPkgStatus") Object freecessPkgStatus0, StringBuilder stringBuilder0) throws Throwable {
        log.i("freezeOLAFPackage " + stringBuilder0 + ", " + frida_helper.object_2_string(freecessPkgStatus0) + ", " + Log.getStackTraceString(new Exception()));
        CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public boolean freezePackage(XC_MethodHook.MethodHookParam params, String s, int v, String s1, int v1, boolean z, boolean z1) throws Throwable {
        log.i("freezePackage1 " + s + ", " + Log.getStackTraceString(new Exception()));
        boolean result = (boolean) CallOriginalMethod(params);
        return result;
    }
}