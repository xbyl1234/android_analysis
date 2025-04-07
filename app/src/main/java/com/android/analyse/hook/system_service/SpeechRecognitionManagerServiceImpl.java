package com.android.analyse.hook.system_service;

import android.content.ComponentName;
import android.os.IBinder;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.speech.SpeechRecognitionManagerServiceImpl")
public class SpeechRecognitionManagerServiceImpl extends FakeClassBase {
    static SpeechRecognitionManagerServiceImpl self;

    static public void Hook(ClassLoader classLoader) {
        self = new SpeechRecognitionManagerServiceImpl();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    void createSessionLocked(XC_MethodHook.MethodHookParam params,
                             ComponentName componentName,
                             IBinder clientToken,
                             boolean onDevice,
                             @FakeParams(ClassName = "android.speech.IRecognitionServiceManagerCallback") Object callback) throws Throwable {
        log.i("createSessionLocked: " + componentName);
        CallOriginalMethod(params);
    }
}
