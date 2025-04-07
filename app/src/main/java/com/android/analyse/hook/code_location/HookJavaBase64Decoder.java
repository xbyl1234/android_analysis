package com.android.analyse.hook.code_location;

import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import java.util.Base64;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "java.util.Base64$Decoder")
public class HookJavaBase64Decoder extends FakeClassBase {
    static public void Hook(ClassLoader classLoader) {
        Hooker.HookClass(classLoader, new HookJavaBase64Decoder());
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    private int decode0(XC_MethodHook.MethodHookParam params, byte[] src, int sp, int sl, byte[] dst) throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("java base64 decode", null, src, dst);
        return result;
    }
}
