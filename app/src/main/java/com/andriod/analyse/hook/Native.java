package com.andriod.analyse.hook;

import com.common.log;

public class Native {

    public static native boolean nativeInitJniTrace(Class fridaHelper);

    public static native void nativeEnumSymbols();

//    public static native void nativeInitHook(String logPath);

    public static native void nativeLog(int level, String msg);

    public static native long nativeOpenFile(String name);

    public static native void nativeWrite(long handle, String data);

    public static native void nativeWrite(long handle, byte[] data);

    static void LoadAnalyseSo() {
        try {
            System.load("/data/libanalyse.so");
            log.SetLogger(new log.LogWriter() {
                @Override
                public void log(int level, String msg) {
                    nativeLog(level, msg);
                }
            });
        } catch (Throwable e) {
            log.i("load analyse lib error: " + e);
        }
    }
}
