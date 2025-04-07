package com.android.analyse.hook.code_location;

import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Proxy;
import java.net.SocketAddress;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "java.net.Socket")
public class HookSocket extends FakeClassBase {
    static HookSocket self;

    static public void Hook(ClassLoader classLoader) {
        self = new HookSocket();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(constructor = true, hookAll = true)
    public void Socket() {

    }


    public void connect(SocketAddress endpoint, int timeout) throws IOException {

    }
}
