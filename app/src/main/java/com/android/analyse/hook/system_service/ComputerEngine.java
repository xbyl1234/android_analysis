package com.android.analyse.hook.system_service;

import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;


@FakeClass(ClassName = "com.android.server.pm.ComputerEngine")
public class ComputerEngine extends FakeClassBase {
    static ComputerEngine self;

    static public void Hook(ClassLoader classLoader) {
        self = new ComputerEngine();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }
}
