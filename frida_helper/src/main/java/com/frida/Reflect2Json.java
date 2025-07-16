package com.frida;

import android.app.Activity;
import android.app.Application;
import android.content.ContentProvider;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.Signature;
import android.os.BaseBundle;
import android.os.Bundle;
import android.util.ArrayMap;
import android.util.Log;

import com.alibaba.fastjson2.JSONArray;
import com.alibaba.fastjson2.JSONObject;

import java.io.File;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexClassLoader;
import dalvik.system.InMemoryDexClassLoader;
import dalvik.system.PathClassLoader;

public class Reflect2Json {

    static public interface Serialize {
        Object write(Config config, Class clz, Object value) throws Throwable;
    }


    static public class Config {
        public boolean serializeOnlyStatic;
        public boolean serializeStatic;
        public boolean serializePrivate;
        public boolean serializeSupper;
        public int serializeDeep;
        Set<Long> hashCodeList = new HashSet<>();

        public boolean allowSupper() {
            return serializeSupper;
        }

        public boolean checkAllow(Field field) {
            int modifiers = field.getModifiers();
            if (!serializeStatic && Modifier.isStatic(modifiers)) {
                return false;
            }
            if (!serializePrivate && !Modifier.isPublic(modifiers)) {
                return false;
            }
            if (serializeOnlyStatic && !Modifier.isStatic(modifiers)) {
                return false;
            }
            return true;
        }
    }

    public static Class FindClass(String clasName) {
        try {
            return Class.forName(clasName);
        } catch (Throwable e) {
            log.e("not find class " + clasName);
            e.printStackTrace();
            return null;
        }
    }

    public static String warpKey(String key, Class clz) {
        if (clz == null) {
            return key;
        }
        if (isBaseType(clz) || isArray(clz) || isMap(clz)) {
            return key;
        }
        return key + "_" + clz.getName();
    }

    public static Field GetField(Class _class, String name) throws Throwable {
        Field field = _class.getDeclaredField(name);
        field.setAccessible(true);
        return field;
    }

    public static Object GetFieldValue(Class _class, Object obj, String name) throws Throwable {
        Field field = _class.getDeclaredField(name);
        field.setAccessible(true);
        Object result = field.get(obj);
        return result;
    }

    public static Object GetFieldValue(Field field, Object obj) throws Throwable {
        Object result = field.get(obj);
        return result;
    }

    public static Field GetField(String _className, String name) throws Throwable {
        Class _class = Class.forName(_className);
        Field field = _class.getField(name);
        field.setAccessible(true);
        return field;
    }

    static Serialize toStringSerialize = new Serialize() {
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            if (value == null) {
                return null;
            }
            return value.toString();
        }
    };

    static Serialize nullSerialize = new Serialize() {
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            return null;
        }
    };

    private static final char[] HEX_ARRAY = "0123456789ABCDEF".toCharArray();

    public static String BytesToHex(byte[] bytes) {
        if (bytes == null) {
            return null;
        }
        char[] hexChars = new char[bytes.length * 2];
        for (int j = 0; j < bytes.length; j++) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = HEX_ARRAY[v >>> 4];
            hexChars[j * 2 + 1] = HEX_ARRAY[v & 0x0F];
        }
        return new String(hexChars);
    }

    static Serialize byteArraySerialize = new Serialize() {
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            if (value == null) {
                return null;
            }
            return BytesToHex((byte[]) value);
        }
    };
    static Serialize signatureSerialize = new Serialize() {
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            if (value == null) {
                return null;
            }
            Signature sign = (Signature) value;
            return sign.toCharsString();
        }
    };
    static Serialize BundleSerialize = new Serialize() {
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            if (value == null) {
                return null;
            }
            ArrayMap<String, Object> mMap = (ArrayMap<String, Object>) GetFieldValue(BaseBundle.class, value, "mMap");
            if (mMap == null) {
                return null;
            }
            return new JSONObject(mMap);
        }
    };

    static Map<Class, Serialize> customClzSerializeMaps = new HashMap<Class, Serialize>() {{
        put(Object.class, nullSerialize);
        put(java.util.jar.JarFile.class, nullSerialize);
        put(Context.class, nullSerialize);
        put(java.security.PublicKey.class, nullSerialize);
        put(java.net.InetAddress.class, toStringSerialize);
        put(java.net.Inet4Address.class, toStringSerialize);
        put(java.net.Inet6Address.class, toStringSerialize);
        put(android.net.IpPrefix.class, toStringSerialize);
        put(android.net.DhcpInfo.class, toStringSerialize);
        put(ClassLoader.class, toStringSerialize);
        put(DexClassLoader.class, toStringSerialize);
        put(PathClassLoader.class, toStringSerialize);
        put(BaseDexClassLoader.class, toStringSerialize);
        put(InMemoryDexClassLoader.class, toStringSerialize);
        put(Thread.class, toStringSerialize);
        put(ThreadGroup.class, toStringSerialize);
        put(Package.class, toStringSerialize);
        put(Class.class, toStringSerialize);
        put(File.class, toStringSerialize);
        put(Locale.class, toStringSerialize);
        put(Activity.class, toStringSerialize);
        put(ContentProvider.class, toStringSerialize);
        put(ApplicationInfo.class, toStringSerialize);
        put(PackageInfo.class, toStringSerialize);
        put(Application.class, toStringSerialize);
        put(android.os.Looper.class, toStringSerialize);
        put(android.os.Handler.class, toStringSerialize);
        put(byte[].class, byteArraySerialize);

        put(Signature.class, signatureSerialize);
        put(Bundle.class, BundleSerialize);
        put(BaseBundle.class, BundleSerialize);
    }};

    static Map<Pattern, Serialize> customClzNameRegSerializeMaps = new HashMap<Pattern, Serialize>() {{
//        put(Pattern.compile("X.0K0"), toStringSerialize);
//        put(Pattern.compile("X.00H"), toStringSerialize);
//        put(Pattern.compile("com.facebook.lite.LeanClientApplication"), toStringSerialize);
//        put(Pattern.compile("android.*"), toStringSerialize);
        put(Pattern.compile("com.android.org.conscrypt.OpenSSLRSAPublicKey"), nullSerialize);
        put(Pattern.compile("sun.misc.Cleaner"), nullSerialize);
        put(Pattern.compile("androidx.lifecycle.*"), toStringSerialize);
        put(Pattern.compile("dalvik.*"), toStringSerialize);
        put(Pattern.compile("libcore.io.*"), toStringSerialize);
        put(Pattern.compile("sun.misc.Cleaner"), toStringSerialize);
        put(Pattern.compile("java.lang.ref.*"), toStringSerialize);
        put(Pattern.compile("java.util.jar.JarFile"), toStringSerialize);
        put(Pattern.compile("java.util.concurrent.*"), toStringSerialize);
        put(Pattern.compile("android.safetycenter.*"), toStringSerialize);
        put(Pattern.compile("android.database.sqlite.*"), toStringSerialize);
        put(Pattern.compile("android.hardware.*"), toStringSerialize);
        put(Pattern.compile("android.webkit.*"), toStringSerialize);
        put(Pattern.compile("android.app.ApplicationPackageManager"), toStringSerialize);
        put(Pattern.compile("java.lang.BootClassLoader"), toStringSerialize);
    }};

    static Map<Class, Serialize> bastTypeSerializeMaps = new HashMap<Class, Serialize>() {{
        Serialize baseTypeSerialize = new Serialize() {
            @Override
            public Object write(Config config, Class clz, Object value) throws Throwable {
                return value;
            }
        };
        put(int.class, baseTypeSerialize);
        put(byte.class, baseTypeSerialize);
        put(short.class, baseTypeSerialize);
        put(long.class, baseTypeSerialize);
        put(float.class, baseTypeSerialize);
        put(double.class, baseTypeSerialize);
        put(boolean.class, baseTypeSerialize);
        put(String.class, baseTypeSerialize);
        put(Integer.class, baseTypeSerialize);
        put(Byte.class, baseTypeSerialize);
        put(Short.class, baseTypeSerialize);
        put(Long.class, baseTypeSerialize);
        put(Float.class, baseTypeSerialize);
        put(Double.class, baseTypeSerialize);
        put(Boolean.class, baseTypeSerialize);
    }};

    static public boolean isBaseType(Class cls) {
        if (cls == String.class) {
            return true;
        } else if (cls == byte.class || cls == Byte.class) {
            return true;
        } else if (cls == short.class || cls == Short.class) {
            return true;
        } else if (cls == int.class || cls == Integer.class) {
            return true;
        } else if (cls == long.class || cls == Long.class) {
            return true;
        } else if (cls == float.class || cls == Float.class) {
            return true;
        } else if (cls == double.class || cls == Double.class) {
            return true;
        } else if (cls == boolean.class || cls == Boolean.class) {
            return true;
        }
        return false;
    }

    static public boolean isArray(Class clz) {
        return clz.isArray() || Collection.class.isAssignableFrom(clz);
    }

    static public boolean isMap(Class clz) {
        return Map.class.isAssignableFrom(clz);
    }

    static Serialize objectSerialize = new Serialize() {
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            JSONObject selfJson = null;
            try {
                if (value == null && !config.serializeStatic) {
                    return null;
                }
                long clzHashCode = 0;
                if (clz != null) {
                    clzHashCode = clz.hashCode();
                }
                long objHashCode = 0;
                if (value != null) {
                    objHashCode = value.hashCode();
                }
                long hashCode = ((long) clzHashCode << 32) | objHashCode;
                if (config.hashCodeList.contains(hashCode)) {
                    return "_duplication_object";
                }
                //log.i(clz.getName() + ", hashCode: " + hashCode);
                config.hashCodeList.add(hashCode);
                selfJson = new JSONObject();
                Field[] fields = clz.getDeclaredFields();
                for (Field field : fields) {
                    field.setAccessible(true);
                    String fieldName = field.getName();
                    if (!config.checkAllow(field)) {
                        continue;
                    }
                    Class fieldClz = null;
                    Object fieldValue = GetFieldValue(field, value);
                    if (fieldValue != null) {
                        fieldClz = fieldValue.getClass();
                    } else {
                        fieldClz = field.getType();
                    }
                    Serialize serialize = getSerializeClz(fieldClz);
                    selfJson.put(warpKey(fieldName, fieldClz),
                            serialize.write(config, fieldClz, fieldValue));
                }

                if (config.allowSupper()) {
                    Class supClz = getSuperClass(clz);
                    if (supClz != null) {
                        Serialize serialize = getSerializeClz(supClz);
                        selfJson.put(warpKey("_super", supClz),
                                serialize.write(config, supClz, value));
                    }
                }
//            config.hashCodeList.remove(hashCode);
                return selfJson;
            } catch (Throwable e) {
                log.e("frida_helper: " + clz.getName() + ", " + e);
                e.printStackTrace();
                return selfJson;
            }
        }
    };

    static Serialize listSerialize = new Serialize() {

        abstract class ListLoop {
            abstract void callback(int idx, Object obj) throws Throwable;
        }

        void forInList(Class clz, Object value, ListLoop loop) throws Throwable {
            if (Collection.class.isAssignableFrom(clz)) {
                int idx = 0;
                for (Object obj : (Collection) value) {
                    loop.callback(idx, obj);
                    idx++;
                }
            } else if (clz.isArray()) {
                for (int i = 0; i < Array.getLength(value); i++) {
                    Object obj = Array.get(value, i);
                    loop.callback(i, obj);
                }
            }
        }

        //        @Override
//        public Object write(Config config, Class clz, Object value) throws Throwable {
//            JSONArray jas = new JSONArray();
//            try {
//                if (value == null) {
//                    return null;
//                }
//                forInList(clz, value, new ListLoop() {
//                    @Override
//                    void callback(Object obj) throws Throwable {
//                        if (obj != null) {
//                            Class objClz = obj.getClass();
//                            Serialize serialize = getSerializeClz(objClz);
//                            jas.add(serialize.write(config, objClz, obj));
//                        } else {
//                            jas.add(null);
//                        }
//                    }
//                });
//                return jas;
//            } catch (Throwable e) {
//                log.e("frida_helper: " + clz.getName() + ", " + e);
//                e.printStackTrace();
//                return jas;
//            }
//        }
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            JSONObject jas = new JSONObject();
            try {
                if (value == null) {
                    return null;
                }
                forInList(clz, value, new ListLoop() {
                    @Override
                    void callback(int idx, Object obj) throws Throwable {
                        if (obj != null) {
                            Class objClz = obj.getClass();
                            Serialize serialize = getSerializeClz(objClz);
                            jas.put(warpKey(String.valueOf(idx), obj.getClass()),
                                    serialize.write(config, objClz, obj));
                        } else {
                            jas.put(String.valueOf(idx), null);
                        }
                    }
                });
                return jas;
            } catch (Throwable e) {
                log.e("frida_helper: " + clz.getName() + ", " + e);
                e.printStackTrace();
                return jas;
            }
        }
    };

    static Serialize mapSerialize = new Serialize() {
        @Override
        public Object write(Config config, Class clz, Object value) throws Throwable {
            JSONObject json = null;
            try {
                if (value == null) {
                    return null;
                }
                json = new JSONObject();
                Map map = (Map) value;
                for (Object key : map.keySet()) {
                    Object obj = map.get(key);
                    if (key == null) {
                        if (obj == null) {
                            continue;
                        }
                        json.put("_null", obj.toString());
                        continue;
                    }

                    if (obj != null) {
                        Class objClz = obj.getClass();
                        Serialize serialize = getSerializeClz(objClz);
                        json.put(warpKey(key.toString(), objClz),
                                serialize.write(config, objClz, obj));
                    } else {
                        json.put(key.toString(), null);
                    }
                }
                return json;
            } catch (Throwable e) {
                log.e("frida_helper: " + clz.getName() + ", " + e);
                e.printStackTrace();
                return json;
            }
        }
    };

    public static Class getSuperClass(Class<?> calzz) {
        Class<?> superclass = calzz.getSuperclass();
        if (superclass == Object.class) {
            return null;
        }
        return superclass;
    }

    static Serialize getSerializeClz(Class clz) {
        //log.i("getSerializeClz: " + clz.getName());
        Serialize result = bastTypeSerializeMaps.get(clz);
        if (result != null) {
            return result;
        }
        result = customClzSerializeMaps.get(clz);
        if (result != null) {
            return result;
        }
        for (Pattern item : customClzNameRegSerializeMaps.keySet()) {
            if (item.matcher(clz.getName()).find()) {
                return customClzNameRegSerializeMaps.get(item);
            }
        }

        if (isArray(clz)) {
            return listSerialize;
        }
        if (isMap(clz)) {
            return mapSerialize;
        }
        return objectSerialize;
    }

    public static Object Object2Json(Object obj) throws Throwable {
        if (obj == null) {
            return null;
        }
        Config config = new Config();
        config.serializeDeep = -1;
        config.serializeSupper = true;
        config.serializeStatic = false;
        config.serializePrivate = true;
        config.serializeOnlyStatic = false;
        return Object2Json(config, obj.getClass(), obj);
    }

    public static Object StaticClass2Json(Class clz) throws Throwable {
        Config config = new Config();
        config.serializeDeep = -1;
        config.serializeSupper = true;
        config.serializeStatic = true;
        config.serializePrivate = true;
        config.serializeOnlyStatic = true;
        return Object2Json(config, clz, null);
    }

    public static Object Object2Json(Config config, Class clz, Object obj) throws Throwable {
        try {
            //log.i("Object2Json: " + clz.getName() + ", " + obj);
            Serialize serialize = getSerializeClz(clz);
            Object result = serialize.write(config, clz, obj);
            if (result == null) {
                return null;
            }
            return result;
        } catch (Throwable e) {
            e.printStackTrace();
            throw e;
        }
    }
}
