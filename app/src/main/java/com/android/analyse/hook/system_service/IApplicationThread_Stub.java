package com.android.analyse.hook.system_service;


import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.HookTools;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Map;

import de.robv.android.xposed.XC_MethodHook;


@FakeClass(ClassName = "android.app.IApplicationThread$Stub")
public class IApplicationThread_Stub extends FakeClassBase {
    static IApplicationThread_Stub self;
    static Map<Integer, String> transactName;

    static public void Hook(ClassLoader classLoader) throws Throwable {
        self = new IApplicationThread_Stub();
        Hooker.HookClass(classLoader, self);
        transactName = HookTools.getTransactName("android.app.IApplicationThread$Stub");
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }


    @FakeMethod(needXposedParams = true)
    public boolean onTransact(XC_MethodHook.MethodHookParam params, int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws Throwable {
        log.i("IApplicationThread onTransact: " + transactName.get(code));
        return (boolean) CallOriginalMethod(params);
    }

}
