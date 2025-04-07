package com.android.analyse.hook.code_location;

import android.util.Log;

import com.common.log;
import com.frida.frida_helper;

import java.nio.charset.StandardCharsets;
import java.util.function.IntConsumer;

public class logger {
    public static boolean isReadableUtf8(byte[] data) {
        if (data == null) {
            return false;
        }
        try {
            int[] readable = new int[2];
            String str = new String(data, 0, Math.min(data.length, 100), StandardCharsets.UTF_8);
            str.codePoints().forEach(new IntConsumer() {
                @Override
                public void accept(int value) {
                    if ((value >= 0x20 && value <= 0x7E) ||
                            (value >= 0xA1 && value <= 0xFF) ||
                            (value >= 0x4E00 && value <= 0x9FFF) ||
                            (value == '\n') ||
                            (value == '\r') ||
                            (value == '\t')) {
                        readable[0] += 1;
                    } else {
                        readable[1] += 1;
                    }
                }
            });
            return (double) readable[0] / (double) (readable[0] + readable[1]) > 0.9;
        } catch (Exception e) {
            log.e("isReadableUtf8 " + e);
            return false;
        }
    }

    static void logEncodeDecode(String tags, Object obj, byte[] input, byte[] output) {
        String inputObj;
        String outputObj;
        if (isReadableUtf8(input)) {
            inputObj = new String(input);
        } else {
            inputObj = frida_helper.object_2_string(input);
        }
        if (isReadableUtf8(output)) {
            outputObj = new String(output);
        } else {
            outputObj = frida_helper.object_2_string(output);
        }
        log.i(tags + ": " + obj + " input " + inputObj + ", output " +
                outputObj + "\n" + Log.getStackTraceString(new Throwable()));
    }
}
