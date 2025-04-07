package com.android.analyse.hook.system_service;

import android.os.Message;
import android.os.MessageQueue;
import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "android.os.Handler")
public class HandleHooker extends FakeClassBase {

    static HandleHooker self;

    static public void Hook(ClassLoader classLoader) {
        self = new HandleHooker();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    public void doLogs(Runnable r) {
        log.i("post: " + r + ", " + Log.getStackTraceString(new Exception()));
    }

    @FakeMethod(needXposedParams = true)
    public boolean enqueueMessage(XC_MethodHook.MethodHookParam params, MessageQueue queue,
                                  Message msg, long uptimeMillis) throws Throwable {
        String logs = "";
        logs += "what: " + msg.what + ", ";
        logs += "obj: " + msg.obj + ", ";
        logs += "callback: " + msg.getCallback() + ", ";
        log.i("handle post: " + logs);
        return (boolean) CallOriginalMethod(params);
    }


    @FakeMethod(needXposedParams = true)
    public final boolean post(XC_MethodHook.MethodHookParam params, Runnable r) throws Throwable {
        doLogs(r);
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public final boolean postAtFrontOfQueue(XC_MethodHook.MethodHookParam params, Runnable r) throws Throwable {
        doLogs(r);
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public final boolean postAtTime(XC_MethodHook.MethodHookParam params, Runnable r, long uptimeMillis) throws Throwable {
        doLogs(r);
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public final boolean postAtTime(XC_MethodHook.MethodHookParam params, Runnable r, Object token, long uptimeMillis) throws Throwable {
        doLogs(r);
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public final boolean postDelayed(XC_MethodHook.MethodHookParam params, Runnable r, int what, long delayMillis) throws Throwable {
        doLogs(r);
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public final boolean postDelayed(XC_MethodHook.MethodHookParam params, Runnable r, long delayMillis) throws Throwable {
        doLogs(r);
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public final boolean postDelayed(XC_MethodHook.MethodHookParam params, Runnable r, Object token, long delayMillis) throws Throwable {
        doLogs(r);
        return (boolean) CallOriginalMethod(params);
    }

}
