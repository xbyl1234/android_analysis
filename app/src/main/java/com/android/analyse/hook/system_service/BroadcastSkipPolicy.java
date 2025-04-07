package com.android.analyse.hook.system_service;

import android.content.pm.ResolveInfo;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.callbacks.XC_LoadPackage;


@FakeClass(ClassName = "com.android.server.am.BroadcastSkipPolicy")
public class BroadcastSkipPolicy extends FakeClassBase {
    static BroadcastSkipPolicy self;

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    static public void Hook(XC_LoadPackage.LoadPackageParam lpparam) {
        self = new BroadcastSkipPolicy();
        Hooker.HookClass(lpparam.classLoader, self);
    }

    @FakeMethod(needXposedParams = true)
    String shouldSkipMessage(@FakeParams(ClassName = "com.android.server.am.BroadcastRecord") Object r,
                             ResolveInfo info) {
        log.i("shouldSkipMessage: " + info);
        return null;
    }

    @FakeMethod(needXposedParams = true)
    String shouldSkipMessage(@FakeParams(ClassName = "com.android.server.am.BroadcastRecord") Object r,
                             @FakeParams(ClassName = "com.android.server.am.BroadcastFilter") Object filter) {
        log.i("shouldSkipMessage: " + filter);
        return null;
    }
}
