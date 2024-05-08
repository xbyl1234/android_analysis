package com.common;


import android.util.Log;

public class log {
    public interface LogWriter {
        void log(int level, String msg);
    }

    static public class LogLevel {
        public final static int Debug = 0;
        public final static int Info = 1;
        public final static int Waring = 2;
        public final static int Error = 3;
        final static String LogTag[] = {
                "Debug",
                "Info",
                "Waring",
                "Error"
        };
    }

    static String Tag = "analyse_log";
    static int LOGMAXLENGTH = 1024;

    static LogWriter writer = null;

    static public void SetLogger(LogWriter w) {
        writer = w;
    }

    public static void AdbLog(int level, String msg) {
        switch (level) {
            case LogLevel.Debug:
                Log.d(Tag, msg);
                break;
            case LogLevel.Info:
                Log.i(Tag, msg);
                break;
            case LogLevel.Waring:
                Log.w(Tag, msg);
                break;
            case LogLevel.Error:
                Log.e(Tag, msg);
                break;
        }
    }

    synchronized public static void unLimitAdbLog(int level, String msg) {
        int strLength = msg.length();
        int start = 0;
        int end = LOGMAXLENGTH;
        for (int i = 0; i < 100; i++) {
            if (strLength > end) {
                AdbLog(level, msg.substring(start, end));
                start = end;
                end = end + LOGMAXLENGTH;
            } else {
                AdbLog(level, msg.substring(start, strLength));
                break;
            }
        }
    }

    public static void LogNoWriter(int level, String msg) {
        Log(level, msg, false);
    }

    public static void Log(int level, String msg, boolean canWriter) {
        msg = LogLevel.LogTag[level] + "\t" + Tag + ":\t\t\t" + msg;
        unLimitAdbLog(level, msg);
        if (canWriter && writer != null) {
            writer.log(level, msg);
        }
    }

    public static void d(String msg) {
        Log(LogLevel.Debug, msg, true);
    }

    public static void i(String msg) {
        Log(LogLevel.Info, msg, true);
    }

    public static void w(String msg) {
        Log(LogLevel.Waring, msg, true);
    }

    public static void e(String msg) {
        Log(LogLevel.Error, msg, true);
    }
}
