package com.andriod.analyse.hook;

import com.common.log;

public class AppFileWriter {
    long handle;

    public AppFileWriter(String name) {
        handle = Native.nativeOpenFile(name);
    }

    public void write(String data) {
        log.i(data);
        Native.nativeWrite(handle, data);
    }
}
