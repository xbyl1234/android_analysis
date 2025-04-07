package com.android.analyse.hook.code_location;

import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "java.nio.channels.SocketChannel")
public class HookSocketChannel  extends FakeClassBase {
    static HookSocketChannel self;

    static public void Hook(ClassLoader classLoader) {
        self = new HookSocketChannel();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

}
