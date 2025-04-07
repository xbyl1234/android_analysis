package com.android.analyse.hook.code_location;

import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;

import java.net.InetAddress;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "java.net.InetSocketAddress")
public class HookInetSocketAddress extends FakeClassBase {
    static HookInetSocketAddress self;

    static public void Hook(ClassLoader classLoader) {
        self = new HookInetSocketAddress();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(constructor = true, hookAll = true)
    public void InetSocketAddress() {

    }
}
