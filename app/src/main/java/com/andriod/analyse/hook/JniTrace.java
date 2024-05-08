package com.andriod.analyse.hook;

public class JniTrace {

    public static boolean InitJniTrace(Class fridaHelper) {
        return Native.nativeInitJniTrace(fridaHelper);
    }
}
