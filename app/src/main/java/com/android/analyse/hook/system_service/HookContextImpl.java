package com.android.analyse.hook.system_service;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.os.UserHandle;
import android.os.Handler;
import android.content.ComponentName;
import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.Executor;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "android.app.ContextImpl")
public class HookContextImpl extends FakeClassBase {
    static HookContextImpl self;

    static public void Hook(ClassLoader classLoader) {
        self = new HookContextImpl();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    //    ComponentName findComponentName(Context ctx, Intent service) {
//        List<ResolveInfo> services = ctx.getPackageManager()
//                .queryIntentServices(service, PackageManager.GET_META_DATA);
//        if (services.isEmpty()) {
//            return null;
//        }
//        ResolveInfo resolveInfo = services.get(0);
//        return new ComponentName(resolveInfo.serviceInfo.packageName, resolveInfo.serviceInfo.name);
//    }
//
    List<String> disable = Arrays.asList(
//            "android.adservices.MEASUREMENT_SERVICE",
//            "com.google.android.gms.safetynet.service.START",
//            "com.google.android.chimera.BoundService.START",
//            "com.google.android.gms.clearcut.service.START",
//            "com.google.android.gms.usagereporting.service.START"
//            "com.google.android.gms.measurement.START"
    );

    boolean isDisable(String act) {
        if (act == null) {
            return false;
        }
        for (String item : disable) {
            if (act.contains(item)) {
                return true;
            }
        }
        return false;
    }
//
//    @FakeMethod(needXposedParams = true)
//    Intent registerReceiverInternal(XC_MethodHook.MethodHookParam params, BroadcastReceiver receiver,
//                                    int userId, IntentFilter filter, String broadcastPermission,
//                                    Handler scheduler, Context context, int flags) throws Throwable {
//        log.i("registerReceiverInternal filter " + frida_helper.object_2_string(filter));
//        if (filter.hasAction("com.google.android.play.core.splitinstall.receiver.SplitInstallUpdateIntentService")) {
//            log.i("registerReceiverInternal " + receiver.getClass() + ", " + filter);
//        }
//        return (Intent) CallOriginalMethod(params);
//    }

    @FakeMethod(needXposedParams = true)
    private ComponentName startServiceCommon(XC_MethodHook.MethodHookParam params, Intent service,
                                             boolean requireForeground, UserHandle user) throws Throwable {
        String action = service.getAction();
        log.i("start: " + action);
//      if (action.contains("com.google.android.gms.ads.identifier.service.START")) {
//          log.i("fake start");
//          return findComponentName(((Context) params.thisObject), service);
//      }
      if (isDisable(action)) {
          log.i("disable " + action);
          return null;
      }
        return (ComponentName) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true, maxSdk = 33)
    private boolean bindServiceCommon(XC_MethodHook.MethodHookParam params, Intent service, ServiceConnection conn,
                                      int flags, String instanceName, Handler handler, Executor executor, UserHandle user) throws Throwable {
        String action = service.getAction();
        log.i("bind: " + action);
//        if (action.contains("com.google.android.gms.ads.identifier.service.START")) {
//            conn.onServiceConnected(findComponentName(((Context) params.thisObject), service), new FakeGAidService("gaid"));
//            return true;
//        }
        if (action == null) {
            log.i("null " + service);
        }
        if (isDisable(action)) {
            log.i("disable " + action + ", " + Log.getStackTraceString(new Throwable()));
            return false;
        }
        return (boolean) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true, minSdk = 34)
    private boolean bindServiceCommon(XC_MethodHook.MethodHookParam params, Intent service, ServiceConnection conn,
                                      long flags, String instanceName, Handler handler, Executor executor, UserHandle user) throws Throwable {
        String action = service.getAction();
        log.i("bind: " + action);
//        if (action.contains("com.google.android.gms.ads.identifier.service.START")) {
//            conn.onServiceConnected(findComponentName(((Context) params.thisObject), service), new FakeGAidService("gaid"));
//            return true;
//        }
        if (action == null) {
            log.i("null " + service);
        }
        if (isDisable(action)) {
            log.i("disable " + action + ", " + Log.getStackTraceString(new Throwable()));
            return false;
        }
        return (boolean) CallOriginalMethod(params);
    }


    void logStartActivity(Intent... intents) {
        String logs = "";
        for (Intent i : intents) {
            logs += frida_helper.object_2_string(i) + ", ";
        }
        log.i("StartActivity: " + logs + "\n" + Log.getStackTraceString(new Throwable()));
    }

    @FakeMethod(needXposedParams = true)
    public int startActivitiesAsUser(XC_MethodHook.MethodHookParam params, Intent[] intents, Bundle options, UserHandle userHandle) throws Throwable {
        logStartActivity(intents);
        return (int) CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public void startActivities(XC_MethodHook.MethodHookParam params, Intent[] intents, Bundle options) throws Throwable {
        logStartActivity(intents);
        CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public void startActivityAsUser(XC_MethodHook.MethodHookParam params, Intent intent, Bundle options, UserHandle user) throws Throwable {
        logStartActivity(intent);
        CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public void startActivity(XC_MethodHook.MethodHookParam params, Intent intent, Bundle options) throws Throwable {
        logStartActivity(intent);
        CallOriginalMethod(params);
    }

}