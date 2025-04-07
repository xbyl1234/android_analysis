package com.android.analyse.hook.system_service;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "android.app.IActivityTaskManager$Stub$Proxy")
public class IActivityTaskManagerStubProxy extends FakeClassBase {
    static IActivityTaskManagerStubProxy self;

    static public void Hook(ClassLoader classLoader) {
        self = new IActivityTaskManagerStubProxy();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    String makeLog(Intent... intents) {
        String log = "";
        for (Intent item : intents) {
            log += item + ", ";
        }
        log += "\n" + Log.getStackTraceString(new Throwable());
        return log;
    }

    @FakeMethod(needXposedParams = true)
    public int startActivities(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object iApplicationThread0,
                               String s, String s1, Intent[] arr_intent, String[] arr_s, IBinder iBinder0, Bundle bundle0, int v) throws Throwable {
        log.i("startActivities: " + makeLog(arr_intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivity(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                      String callingFeatureId, Intent intent, String resolvedType,
                      IBinder resultTo, String resultWho, int requestCode,
                      int flags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle options) throws Throwable {
        log.i("startActivity: " + makeLog(intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    Object startActivityAndWait(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                                String callingFeatureId, Intent intent, String resolvedType,
                                IBinder resultTo, String resultWho, int requestCode, int flags,
                                @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle options, int userId) throws Throwable {
        log.i("startActivityAndWait: " + makeLog(intent));
        return CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityAsCaller(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                              Intent intent, String resolvedType, IBinder resultTo, String resultWho,
                              int requestCode, int flags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle options,
                              boolean ignoreTargetSecurity, int userId) throws Throwable {
        log.i("startActivityAsCaller: " + makeLog(intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityAsUser(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                            String callingFeatureId, Intent intent, String resolvedType,
                            IBinder resultTo, String resultWho, int requestCode, int flags,
                            @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle options, int userId) throws Throwable {
        log.i("startActivityAsUser: " + makeLog(intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityFromGameSession(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                                     String callingFeatureId, int callingPid, int callingUid, Intent intent,
                                     int taskId, int userId) throws Throwable {
        log.i("startActivityFromGameSession: " + makeLog(intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityFromRecents(XC_MethodHook.MethodHookParam params, int taskId, Bundle options) throws Throwable {
        log.i("startActivityFromRecents: " + taskId + ", " + options);
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityIntentSender(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller,
                                  @FakeParams(ClassName = "android.content.IIntentSender") Object target, IBinder whitelistToken, Intent fillInIntent,
                                  String resolvedType, IBinder resultTo, String resultWho, int requestCode,
                                  int flagsMask, int flagsValues, Bundle options) throws Throwable {
        log.i("startActivityIntentSender: " + makeLog(fillInIntent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityWithConfig(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                                String callingFeatureId, Intent intent, String resolvedType,
                                IBinder resultTo, String resultWho, int requestCode, int startFlags,
                                Configuration newConfig, Bundle options, int userId) throws Throwable {
        log.i("startActivityWithConfig: " + makeLog(intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startAssistantActivity(XC_MethodHook.MethodHookParam params, String callingPackage, String callingFeatureId, int callingPid,
                               int callingUid, Intent intent, String resolvedType, Bundle options, int userId) throws Throwable {
        log.i("startAssistantActivity: " + makeLog(intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    boolean startDreamActivity(XC_MethodHook.MethodHookParam params, Intent intent) throws Throwable {
        log.i("startDreamActivity: " + makeLog(intent));
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    boolean startNextMatchingActivity(XC_MethodHook.MethodHookParam params, IBinder callingActivity,
                                      Intent intent, Bundle options) throws Throwable {
        log.i("startNextMatchingActivity: " + makeLog(intent));
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    void startRecentsActivity(XC_MethodHook.MethodHookParam params, Intent intent, long eventTime,
                              @FakeParams(ClassName = "android.view.IRecentsAnimationRunner") Object recentsAnimationRunner) throws Throwable {
        log.i("startRecentsActivity: " + makeLog(intent));
        CallOriginalMethod(params);
    }
}
