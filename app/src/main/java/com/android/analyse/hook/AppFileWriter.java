package com.android.analyse.hook;

import android.util.Log;
import com.common.log;

public class AppFileWriter {
    long handle;

    public AppFileWriter(String name) {
        handle = Native.nativeOpenFile(name);
    }

    public void write(String data) {
        Log.i("analyse_log", "len: " + data.length() + ", " + data);
        log.i(data);
        Native.nativeWrite(handle, data);
    }
}
