package com.android.analyse.hook.system_service;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.IBinder;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

@FakeClass(ClassName = "com.android.server.wm.ActivityTaskManagerService")
public class ActivityTaskManagerService extends FakeClassBase {
    static ActivityTaskManagerService self;

    static public void Hook(XC_LoadPackage.LoadPackageParam lpparam) {
        if (!(lpparam.processName.equals("android") && (lpparam.packageName.equals("android") ||
                lpparam.packageName.equals("com.android.providers.settings")))) {
            return;
        }

        if (lpparam.packageName.equals("android")) {
            self = new ActivityTaskManagerService();
            Hooker.HookClass(lpparam.classLoader, self);
        }
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    String makeLog(String callingPackage, Intent... intents) {
        String log = "";
        log += "pkg name: " + callingPackage + ", ";
        for (Intent item : intents) {
            log += frida_helper.object_2_string(item) + ", ";
        }
//        log += "\n" + Log.getStackTraceString(new RemoteException());
        return log;
    }

    @FakeMethod(needXposedParams = true)
    public void moveTaskToFront(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object appThread,
                                String callingPackage,
                                int taskId,
                                int flags,
                                Bundle bOptions) throws Throwable {
        log.i("moveTaskToFront  " + callingPackage + ", " + taskId + ", " + flags);
        CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivities(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                        String callingFeatureId, Intent[] intents, String[] resolvedTypes, IBinder resultTo,
                        Bundle bOptions, int userId) throws Throwable {
        log.i("startActivities: " + makeLog(callingPackage, intents));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityAsUser(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                            String callingFeatureId, Intent intent, String resolvedType,
                            IBinder resultTo, String resultWho, int requestCode, int startFlags,
                            @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle bOptions, int userId, boolean validateIncomingUser) throws Throwable {
        log.i("startActivityAsUser: " + makeLog(callingPackage, intent));
        return (int) CallOriginalMethod(params);
    }

    //android.app.WaitResult
    @FakeMethod(needXposedParams = true)
    public final Object startActivityAndWait(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                                             String callingFeatureId, Intent intent, String resolvedType, IBinder resultTo,
                                             String resultWho, int requestCode, int startFlags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo,
                                             Bundle bOptions, int userId) throws Throwable {
        log.i("startActivityAndWait: " + makeLog(callingPackage, intent));
        return CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityWithConfig(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                                String callingFeatureId, Intent intent, String resolvedType, IBinder resultTo,
                                String resultWho, int requestCode, int startFlags, Configuration config,
                                Bundle bOptions, int userId) throws Throwable {
        log.i("startActivityWithConfig: " + makeLog(callingPackage, intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityAsCaller(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                              Intent intent, String resolvedType, IBinder resultTo, String resultWho, int requestCode,
                              int startFlags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle bOptions,
                              boolean ignoreTargetSecurity, int userId) throws Throwable {
        log.i("startActivityAsCaller: " + makeLog(callingPackage, intent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityFromGameSession(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
                                     String callingFeatureId, int callingPid, int callingUid, Intent intent, int taskId,
                                     int userId) throws Throwable {
        log.i("startActivityFromGameSession: " + makeLog(callingPackage, intent));
        return (int) CallOriginalMethod(params);
    }


    @FakeMethod(needXposedParams = true)
    int startActivityIntentSender(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, @FakeParams(ClassName = "android.content.IIntentSender") Object target,
                                  IBinder allowlistToken, Intent fillInIntent, String resolvedType, IBinder resultTo,
                                  String resultWho, int requestCode, int flagsMask, int flagsValues, Bundle bOptions) throws Throwable {
        log.i("startActivityIntentSender: " + makeLog(caller + "", fillInIntent));
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    int startActivityFromRecents(XC_MethodHook.MethodHookParam params, int taskId, Bundle bOptions) throws Throwable {
        log.i("startActivityFromRecents: " + taskId);
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public boolean startNextMatchingActivity(XC_MethodHook.MethodHookParam params, IBinder callingActivity, Intent intent,
                                             Bundle bOptions) throws Throwable {
        log.i("startNextMatchingActivity: " + makeLog(callingActivity + "", intent));
        return (boolean) CallOriginalMethod(params);
    }
}
