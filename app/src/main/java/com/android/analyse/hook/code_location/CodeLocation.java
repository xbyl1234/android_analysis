package com.android.analyse.hook.code_location;

public class CodeLocation {
    public static void hookEncodeDecode(ClassLoader classLoader) {
        HookBase64.Hook(classLoader);
        HookCipher.Hook(classLoader);
        HookJavaBase64Decoder.Hook(classLoader);
        HookJavaBase64Encoder.Hook(classLoader);
    }
}
