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

@FakeClass(ClassName = "com.android.server.display.DisplayManagerService")
public class DisplayManagerService extends FakeClassBase {
    static DisplayManagerService self;

    static public void Hook(ClassLoader classLoader) {
        self = new DisplayManagerService();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod()
    private int createVirtualDisplayInternal(VirtualDisplayConfig virtualDisplayConfig,
                                             @FakeParams(ClassName = "android.hardware.display.IVirtualDisplayCallback") Object callback,
                                             @FakeParams(ClassName = "android.media.projection.IMediaProjection") Object projection,
                                             @FakeParams(ClassName = "android.companion.virtual.IVirtualDevice") Object virtualDevice,
                                             @FakeParams(ClassName = "android.window.DisplayWindowPolicyController") Object dwpc,
                                             String packageName) {
        log.i("createVirtualDisplayInternal: " + packageName);
//        throw new SecurityException("hhhh");
        return Display.INVALID_DISPLAY;
    }
}
