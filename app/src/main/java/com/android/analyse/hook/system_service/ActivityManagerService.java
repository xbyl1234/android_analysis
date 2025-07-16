package com.android.analyse.hook.system_service;

import android.app.BroadcastOptions;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.os.Bundle;
import android.os.IBinder;
import android.os.ParcelFileDescriptor;
import android.os.RemoteException;
import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import java.util.function.BiFunction;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.am.ActivityManagerService")
public class ActivityManagerService extends FakeClassBase {
    static ActivityManagerService self;

    static public void Hook(ClassLoader classLoader) {
        self = new ActivityManagerService();
        Hooker.HookClass(classLoader, self);
    }

//    @FakeMethod(needXposedParams = true)
//    public ParcelFileDescriptor openContentUri(XC_MethodHook.MethodHookParam params, String uriString) throws Throwable {
//        log.i("openContentUri: " + uriString);
//        return (ParcelFileDescriptor) CallOriginalMethod(params);
//    }
//
//    @FakeMethod(needXposedParams = false)
//    void checkExcessivePowerUsage() {
//        log.i("start check cpu " + Log.getStackTraceString(new Exception()));
//    }
//
//    @FakeMethod(needXposedParams = true)
//    public int startActivity(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
//                             Intent intent, String resolvedType, IBinder resultTo, String resultWho, int requestCode,
//                             int startFlags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle bOptions) throws Throwable {
//        log.i("startActivity caller: " + callingPackage + ", intent: " + intent);
//        return (int) CallOriginalMethod(params);
//    }
//
//    @FakeMethod(needXposedParams = true)
//    public int startActivityWithFeature(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
//                                        String callingFeatureId, Intent intent, String resolvedType, IBinder resultTo,
//                                        String resultWho, int requestCode, int startFlags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo,
//                                        Bundle bOptions) throws Throwable {
//        log.i("startActivity caller: " + callingPackage + ", intent: " + intent);
//        return (int) CallOriginalMethod(params);
//    }
//
//    @FakeMethod(needXposedParams = true)
//    public final int startActivityAsUser(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
//                                         Intent intent, String resolvedType, IBinder resultTo, String resultWho, int requestCode,
//                                         int startFlags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle bOptions, int userId) throws Throwable {
//        log.i("startActivity caller: " + callingPackage + ", intent: " + intent);
//        return (int) CallOriginalMethod(params);
//    }
//
//    @FakeMethod(needXposedParams = true)
//    public final int startActivityAsUserWithFeature(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller,
//                                                    String callingPackage, String callingFeatureId, Intent intent, String resolvedType,
//                                                    IBinder resultTo, String resultWho, int requestCode, int startFlags,
//                                                    @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo, Bundle bOptions, int userId) throws Throwable {
//        log.i("startActivity caller: " + callingPackage + ", intent: " + intent);
//        return (int) CallOriginalMethod(params);
//
//    }
//
//    @FakeMethod(needXposedParams = true)
//    Object startActivityAndWait(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller, String callingPackage,
//                                String callingFeatureId, Intent intent, String resolvedType, IBinder resultTo,
//                                String resultWho, int requestCode, int startFlags, @FakeParams(ClassName = "android.app.ProfilerInfo") Object profilerInfo,
//                                Bundle bOptions, int userId) throws Throwable {
//        log.i("startActivity caller: " + callingPackage + ", intent: " + intent);
//        return CallOriginalMethod(params);
//    }
//
//    @FakeMethod(needXposedParams = true)
//    public final int startActivityFromRecents(XC_MethodHook.MethodHookParam params, int taskId, Bundle bOptions) throws Throwable {
//        log.i("startActivity taskId: " + taskId + ", bOptions: " + bOptions);
//        return (int) CallOriginalMethod(params);
//    }

//    @FakeMethod()
//    public boolean validateAssociationAllowedLocked(String pkg1, int uid1, String pkg2, int uid2) {
//        log.i("validateAssociationAllowedLocked: " + pkg1 + ", " + pkg2);
//        return true;
//    }

//    @FakeMethod(needXposedParams = true)
//    final Object startProcessLocked(XC_MethodHook.MethodHookParam params, String processName, ApplicationInfo info,
//                                    boolean knownToBeDead, int intentFlags,
//                                    @FakeParams(ClassName = "com.android.server.am.HostingRecord")
//                                    Object hostingRecord, int zygotePolicyFlags,
//                                    boolean allowWhileBooting, boolean isolated) throws Throwable {
//        log.i("startProcessLocked " + processName + ", " +
//                frida_helper.object_2_string(hostingRecord) + ", " + Log.getStackTraceString(new Throwable()));
//        return CallOriginalMethod(params);
//    }

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
    public final int broadcastIntentWithFeature(XC_MethodHook.MethodHookParam params, @FakeParams(ClassName = "android.app.IApplicationThread") Object caller,
                                                String callingFeatureId, Intent intent, String resolvedType,
                                                @FakeParams(ClassName = "android.content.IIntentReceiver") Object resultTo,
                                                int resultCode, String resultData, Bundle resultExtras,
                                                String[] requiredPermissions, String[] excludedPermissions,
                                                String[] excludedPackages, int appOp, Bundle bOptions,
                                                boolean serialized, boolean sticky, int userId) throws Throwable {
        if (intent != null && intent.getAction().contains("com.google.android.play.core.splitinstall.receiver.SplitInstallUpdateIntentService")) {
            log.i("broadcastIntentWithFeature " + intent);
            return 0;
        }
        return (int) CallOriginalMethod(params);
    }

//    final int broadcastIntentLockedTraced(ProcessRecord callerApp, String callerPackage,
//                                          String callerFeatureId, Intent intent, String resolvedType,
//                                          ProcessRecord resultToApp, IIntentReceiver resultTo, int resultCode, String resultData,
//                                          Bundle resultExtras, String[] requiredPermissions,
//                                          String[] excludedPermissions, String[] excludedPackages, int appOp,
//                                          BroadcastOptions brOptions, boolean ordered, boolean sticky, int callingPid,
//                                          int callingUid, int realCallingUid, int realCallingPid, int userId,
//                                          BackgroundStartPrivileges backgroundStartPrivileges,
//                                          int[] broadcastAllowList,
//                                          BiFunction<Integer, Bundle, Bundle> filterExtrasForReceiver) {
//
//    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }
}
