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

@FakeClass(ClassName = "android.util.Base64")
public class HookBase64 extends FakeClassBase {
    static public void Hook(ClassLoader classLoader) {
//        Base64.getDecoder().decode()
        Hooker.HookClass(classLoader, new HookBase64());
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    public byte[] encode(XC_MethodHook.MethodHookParam params, byte[] input, int offset, int len, int flags) throws Throwable {
        byte[] result = (byte[]) CallOriginalMethod(params);
        logger.logEncodeDecode("android base64 encode", null, input, result);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public static byte[] decode(XC_MethodHook.MethodHookParam params, byte[] input, int offset, int len, int flags) throws Throwable {
        byte[] result = (byte[]) CallOriginalMethod(params);
        logger.logEncodeDecode("android base64 decode", null, input, result);
        return result;
    }
}
