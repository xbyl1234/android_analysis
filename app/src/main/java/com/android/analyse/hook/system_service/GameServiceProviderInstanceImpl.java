package com.android.analyse.hook.system_service;


import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;


@FakeClass(ClassName = "com.android.server.app.GameServiceProviderInstanceImpl")
public class GameServiceProviderInstanceImpl extends FakeClassBase {
    static GameServiceProviderInstanceImpl self;

    static public void Hook(ClassLoader classLoader) {
        self = new GameServiceProviderInstanceImpl();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    private void gameTaskStartedLocked(XC_MethodHook.MethodHookParam param,
                                       @FakeParams(ClassName = "com.android.server.app.GameTaskInfo") Object gameTaskInfo) throws Throwable {
        log.i("gameTaskStartedLocked: " + gameTaskInfo);
//        CallOriginalMethod(param);
    }
}
