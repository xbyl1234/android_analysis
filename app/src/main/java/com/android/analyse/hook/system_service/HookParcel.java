package com.android.analyse.hook.system_service;

import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "android.os.Parcel")
public class HookParcel extends FakeClassBase {
    static HookParcel self;

    static public void Hook(ClassLoader classLoader) {
        self = new HookParcel();
        Hooker.HookClass(classLoader, self);
    }

    @FakeMethod(needXposedParams = true)
    public final void enforceInterface(XC_MethodHook.MethodHookParam params, String interfaceName) throws Throwable {
        log.i("enforceInterface: " + interfaceName);
        CallOriginalMethod(params);
    }

    @FakeMethod(needXposedParams = true)
    public final void writeInterfaceToken(XC_MethodHook.MethodHookParam params, String interfaceName) throws Throwable {
        log.i("writeInterfaceToken: " + interfaceName);
        CallOriginalMethod(params);
    }


    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }
}
