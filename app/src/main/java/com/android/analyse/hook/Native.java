package com.android.analyse.hook;

import com.common.log;

public class Native {
    public static native void test();

    public static native void initNative(String pkgName);
    public static native boolean nativeInitJniTrace(Class fridaHelper);

    public static native void nativeEnumSymbols();

//    public static native void nativeInitHook(String logPath);

    public static native void nativeLog(int level, String msg);

    public static native long nativeOpenFile(String name);

    public static native void nativeWrite(long handle, String data);

    public static native void nativeWrite(long handle, byte[] data);

    public static native void passFbSslPinning();
    public static native void passInsSslPinning();
    static void LoadAnalyseSo(String pkgName) {
        try {
            System.load("/data/libanalyse.so");
            initNative(pkgName);
//            log.SetLogger(new log.LogWriter() {
//                @Override
//                public void log(int level, String msg) {
//                    nativeLog(level, msg);
//                }
//            });
        } catch (Throwable e) {
            log.i("load analyse lib error: " + e);
        }
    }
}
