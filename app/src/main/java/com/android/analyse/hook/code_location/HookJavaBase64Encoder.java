package com.android.analyse.hook.code_location;

import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "java.util.Base64$Encoder")
public class HookJavaBase64Encoder extends FakeClassBase {
    static public void Hook(ClassLoader classLoader) {
        Hooker.HookClass(classLoader, new HookJavaBase64Encoder());
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    private int encode0(XC_MethodHook.MethodHookParam params, byte[] src, int off, int end, byte[] dst) throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("java base64 encode", null, src, dst);
        return result;
    }
}
