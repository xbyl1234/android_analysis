package com.android.analyse.hook;

public class JniTrace {

    public static boolean InitJniTrace(Class fridaHelper) {
        return Native.nativeInitJniTrace(fridaHelper);
    }
}
