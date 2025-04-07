package com.android.analyse.hook.system_service;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.HookTools;
import com.common.tools.hooker.Hooker;

import java.util.Map;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "android.content.pm.IPackageManager$Stub")
public class IPackageManager_Stub extends FakeClassBase {
    static IPackageManager_Stub self;
    static Map<Integer, String> transactName;

    static public void Hook(ClassLoader classLoader) throws Throwable {
        self = new IPackageManager_Stub();
        Hooker.HookClass(classLoader, self);
        transactName = HookTools.getTransactName("android.content.pm.IPackageManager$Stub");
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    public boolean onTransact(XC_MethodHook.MethodHookParam params, int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws Throwable {
        log.i("IPackageManager onTransact: " + transactName.get(code));
        return (boolean) CallOriginalMethod(params);
    }
}