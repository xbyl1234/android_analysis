package com.android.analyse.hook.code_location;

import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import java.nio.ByteBuffer;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "javax.crypto.Cipher")
public class HookCipher extends FakeClassBase {
    static public void Hook(ClassLoader classLoader) {
        Hooker.HookClass(classLoader, new HookCipher());
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    public final byte[] update(XC_MethodHook.MethodHookParam params, byte[] input) throws Throwable {
        byte[] result = (byte[]) CallOriginalMethod(params);
        logger.logEncodeDecode("update1", params.thisObject, input, result);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final byte[] update(XC_MethodHook.MethodHookParam params, byte[] input, int inputOffset, int inputLen) throws Throwable {
        byte[] result = (byte[]) CallOriginalMethod(params);
        logger.logEncodeDecode("update2", params.thisObject, input, result);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final int update(XC_MethodHook.MethodHookParam params, byte[] input, int inputOffset, int inputLen,
                            byte[] output)
            throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("update3", params.thisObject, input, output);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final int update(XC_MethodHook.MethodHookParam params, byte[] input, int inputOffset, int inputLen,
                            byte[] output, int outputOffset)
            throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("update4", params.thisObject, input, output);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final int update(XC_MethodHook.MethodHookParam params, ByteBuffer input, ByteBuffer output)
            throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("update5", params.thisObject, input.array(), output.array());
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final byte[] doFinal(XC_MethodHook.MethodHookParam params)
            throws Throwable {
        byte[] result = (byte[]) CallOriginalMethod(params);
        logger.logEncodeDecode("doFinal1", params.thisObject, null, result);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final int doFinal(XC_MethodHook.MethodHookParam params, byte[] output, int outputOffset)
            throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("doFinal2", params.thisObject, null, output);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final byte[] doFinal(XC_MethodHook.MethodHookParam params, byte[] input)
            throws Throwable {
        byte[] result = (byte[]) CallOriginalMethod(params);
        logger.logEncodeDecode("doFinal3", params.thisObject, input, result);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final byte[] doFinal(XC_MethodHook.MethodHookParam params, byte[] input, int inputOffset, int inputLen)
            throws Throwable {
        byte[] result = (byte[]) CallOriginalMethod(params);
        logger.logEncodeDecode("doFinal4", params.thisObject, input, result);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final int doFinal(XC_MethodHook.MethodHookParam params, byte[] input, int inputOffset, int inputLen,
                             byte[] output)
            throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("doFinal5", params.thisObject, input, output);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final int doFinal(XC_MethodHook.MethodHookParam params, byte[] input, int inputOffset, int inputLen,
                             byte[] output, int outputOffset)
            throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("doFinal6", params.thisObject, input, output);
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public final int doFinal(XC_MethodHook.MethodHookParam params, ByteBuffer input, ByteBuffer output)
            throws Throwable {
        int result = (int) CallOriginalMethod(params);
        logger.logEncodeDecode("doFinal7", params.thisObject, input.array(), output.array());
        return result;
    }
}
