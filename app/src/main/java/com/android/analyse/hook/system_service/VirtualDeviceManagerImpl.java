package com.android.analyse.hook.system_service;

import android.hardware.display.VirtualDisplayConfig;
import android.os.IBinder;
import android.view.Display;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.companion.virtual.VirtualDeviceManagerService$VirtualDeviceManagerImpl")
public class VirtualDeviceManagerImpl extends FakeClassBase {
    static VirtualDeviceManagerImpl self;

    static public void Hook(ClassLoader classLoader) {
        self = new VirtualDeviceManagerImpl();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod()
    public int createVirtualDisplay(VirtualDisplayConfig virtualDisplayConfig,
                                    @FakeParams(ClassName = "android.hardware.display.IVirtualDisplayCallback") Object callback,
                                    @FakeParams(ClassName = "android.companion.virtual.IVirtualDevice") Object virtualDevice,
                                    String packageName) {
        log.i("createVirtualDisplay: " + packageName);
        throw new SecurityException("hhhh");
    }

    public Object createVirtualDevice(
            IBinder token,
            String packageName,
            int associationId,
            @FakeParams(ClassName = "android.companion.virtual.VirtualDeviceParams") Object params,
            @FakeParams(ClassName = "android.companion.virtual.IVirtualDeviceActivityListener") Object activityListener,
            @FakeParams(ClassName = "android.companion.virtual.IVirtualDeviceSoundEffectListener") Object soundEffectListener) {
        log.i("createVirtualDevice: " + packageName);
        throw new SecurityException("hhhh");
    }
}
