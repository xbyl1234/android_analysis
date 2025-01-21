package com.frida;

import android.annotation.SuppressLint;
import com.fucker.gson.Gson;
import com.fucker.gson.GsonBuilder;
import com.fucker.gson.JsonElement;
import com.fucker.gson.JsonPrimitive;
import com.fucker.gson.JsonSerializationContext;
import com.fucker.gson.JsonSerializer;

import java.lang.reflect.Type;


public class frida_helper {
    static class ByteArraySerializer implements JsonSerializer<byte[]> {
        @Override
        public JsonElement serialize(byte[] src, Type typeOfSrc, JsonSerializationContext context) {
            return new JsonPrimitive(byte_2_hex_str(src));
        }
    }

    public static String object_2_string(Object obj) {
        log.i("on object_2_string, obj is " + obj);
        if (obj == null) {
            return "null";
        }
        String ret = "";
        String json_err = "";
        try {
            Object result = Reflect2Json.Object2Json(obj);
            Gson gson = new GsonBuilder()
                    .registerTypeAdapter(byte[].class, new ByteArraySerializer())
                    .serializeSpecialFloatingPointValues()
                    .create();
            ret = gson.toJson(result);
        } catch (Throwable e) {
            json_err += "Gson error: " + e.toString() + "\n";
            e.printStackTrace();
            log.i(json_err);
        }

        if (!json_err.equals("")) {
            ret = json_err + ret;
        }

        log.i("out object_2_string, obj is " + obj);
        return ret;
    }

    public static String byte_2_hex(byte b) {
        return String.format("%02x", b);
    }

    public static String byte_2_hex_str(Object obj, int off, int len) {
        byte[] by = (byte[]) obj;
        StringBuilder sb = new StringBuilder();
        for (int i = off; i - off < len; i++) {
            sb.append(String.format("%02x", by[i]));
            sb.append(" ");
        }
        return sb.toString();
    }

    public static String byte_2_hex_str(Object obj) {
        return byte_2_hex_str(obj, 0, ((byte[]) obj).length);
    }

    public static String byte_2_string(byte[] by, String decode) {
        try {
            return new String(by, decode);
        } catch (Exception e) {
            return e.toString();
        }
    }

    public static String byte_2_ip_str(byte[] by) {
        StringBuilder sb = new StringBuilder();
        for (byte item : by) {
            sb.append(String.format("%d", (int) (item & 0xff)));
            sb.append(".");
        }
        return sb.toString();
    }

}
