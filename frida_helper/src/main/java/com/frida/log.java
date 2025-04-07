package com.frida;

import android.util.Log;


class log {
    static String tag = "frida_helper";

    public static void i(String msg, boolean result) {
        if (result) {
            Log.i(tag, msg + " success!!!");
        } else {
            Log.i(tag, msg + " error!!!");
        }
    }

    public static void i(String msg) {
        Log.i(tag, msg);
    }

    public static void e(String msg) {
        Log.e(tag, msg);
    }

    public static void e(Throwable e) {
        Log.i(tag, Log.getStackTraceString(e));
    }
}
