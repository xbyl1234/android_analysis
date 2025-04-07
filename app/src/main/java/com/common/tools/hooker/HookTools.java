package com.common.tools.hooker;

import com.common.log;
import com.frida.frida_helper;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XC_MethodReplacement;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;

public class HookTools {
    public static void fastHookAll(ClassLoader classLoader, String clzName, String... methodName) {
        for (String name : methodName) {
            XposedBridge.hookAllMethods(XposedHelpers.findClass(clzName, classLoader), name, new XC_MethodReplacement() {
                @Override
                protected Object replaceHookedMethod(MethodHookParam param) throws Throwable {
                    Object result = XposedBridge.invokeOriginalMethod(param.method, param.thisObject, param.args);
                    log.i("call " + name + " params: " + frida_helper.object_2_string(param.args) + ", result: " + frida_helper.object_2_string(result));
                    return result;
                }
            });
        }
    }

    public static Map<Integer, String> getTransactName(String clzName) throws Throwable {
        Map<Integer, String> result = new HashMap<>();
        Class clz = Class.forName(clzName);
        Field[] fields = clz.getDeclaredFields();
        for (Field f : fields) {
            f.setAccessible(true);
            if (f.getName().startsWith("TRANSACTION_") && f.getType() == int.class) {
                result.put(f.getInt(null), f.getName());
            }
        }
        return result;
    }

    public static Constructor GetConstructor(Class clazz, Class<?>... parameterTypes) {
        try {
            return clazz.getDeclaredConstructor(parameterTypes);
        } catch (Throwable e) {
            log.e("GetConstructor " + clazz.getName() + " error:" + e);
            e.printStackTrace();
            return null;
        }
    }

    public static Constructor GetConstructorThrow(Class clazz, Class<?>... parameterTypes) throws NoSuchMethodException {
        return clazz.getDeclaredConstructor(parameterTypes);
    }

    public static Object CallConstructor(Class clazz, Object... params) {
        try {
            Class[] paramsType = new Class[params.length];
            for (int i = 0; i < params.length; i++) {
                paramsType[i] = params[i].getClass();
            }
            Constructor constructor = GetConstructor(clazz, paramsType);
            constructor.setAccessible(true);
            return constructor.newInstance(params);
        } catch (Throwable e) {
            log.e("CallConstructor " + clazz.getName() + " error:" + e);
            e.printStackTrace();
            return null;
        }
    }

    public static Object CallConstructor(Constructor con, Object... params) {
        try {
            con.setAccessible(true);
            return con.newInstance(params);
        } catch (Throwable e) {
            log.e("CallConstructor " + con.getName() + " error:" + e);
            e.printStackTrace();
            return null;
        }
    }

    public static Object CallMethod(Method method, Object obj, Object... params) {
        try {
            return method.invoke(obj, params);
        } catch (Throwable e) {
            log.e("CallMethod " + method.getName() + " error:" + e);
            e.printStackTrace();
            return null;
        }
    }

    public static Method GetMethodThrow(Class clazz, String methodName, Class<?>... parameterTypes) {
        return XposedHelpers.findMethodExact(clazz, methodName, parameterTypes);
    }

    public static Method GetMethod(Class clazz, String methodName, Class<?>... parameterTypes) {
        try {
            return XposedHelpers.findMethodExact(clazz, methodName, parameterTypes);
        } catch (Throwable e) {
            log.e("GetMethod " + clazz + "." + methodName + " error:" + e);
            e.printStackTrace();
            return null;
        }
    }

    public static void PrintfConstructor(Class clazz) {
        Constructor[] cons = clazz.getDeclaredConstructors();
        for (int i = 0; i < cons.length; i++) {
            log.i("constructor " + i + " " + cons[i].getName());
            for (Class cls : cons[i].getParameterTypes()) {
                log.i("Parameter " + i + " " + cls.getName());
            }
        }
    }

    public static Class FindClass(String clasName, ClassLoader classLoader) {
        try {
            return XposedHelpers.findClass(clasName, classLoader);
        } catch (Throwable e) {
            log.e("not find class " + clasName);
            e.printStackTrace();
            return null;
        }
    }

    public static Class FindClassThrow(String clasName, ClassLoader classLoader) {
        return XposedHelpers.findClass(clasName, classLoader);
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

    public static Field GetFieldThrow(Class _class, String name) throws NoSuchFieldException {
        return _class.getDeclaredField(name);
    }

    public static Field GetField(Class _class, String name) {
        try {
            return _class.getDeclaredField(name);
        } catch (Throwable e) {
            log.e("get field " + name + " error!");
            e.printStackTrace();
            return null;
        }
    }

    public static Object GetFieldValue(Field field, Object obj) {
        try {
            Object result = null;
            boolean oldAcc = field.isAccessible();
            field.setAccessible(true);
            result = field.get(obj);
            field.setAccessible(oldAcc);
            return result;
        } catch (Throwable e) {
            log.e("get field value " + field.getName() + " error!");
            e.printStackTrace();
            return null;
        }
    }

    public static Object GetFieldValue(Class _class, Object obj, String name) {
        try {
            Object result = null;
            Field field = GetField(_class, name);
            if (field == null) {
                return null;
            }
            boolean oldAcc = field.isAccessible();
            field.setAccessible(true);
            result = field.get(obj);
            field.setAccessible(oldAcc);
            return result;
        } catch (Throwable e) {
            log.e("get field value " + name + " error!");
            e.printStackTrace();
            return null;
        }
    }

    public static boolean SetField(Field field, Object obj, Object value) {
        try {
            boolean oldAcc = field.isAccessible();
            field.setAccessible(true);
            field.set(obj, value);
            field.setAccessible(oldAcc);
            return true;
        } catch (Throwable e) {
            log.e("set field value " + field + " error!");
            e.printStackTrace();
            return false;
        }
    }

    public static boolean SetField(Object obj, String name, Object value) {
        try {
            Field field = GetField(obj.getClass(), name);
            if (field == null) {
                return false;
            }
            boolean oldAcc = field.isAccessible();
            field.setAccessible(true);
            field.set(obj, value);
            field.setAccessible(oldAcc);
            return true;
        } catch (Throwable e) {
            log.e("set field value " + name + " error!");
            e.printStackTrace();
            return false;
        }
    }

    public static boolean SetField(Class _class, Object obj, String name, Object value) {
        try {
            Field field = GetField(_class, name);
            if (field == null) {
                return false;
            }
            boolean oldAcc = field.isAccessible();
            field.setAccessible(true);
            field.set(obj, value);
            field.setAccessible(oldAcc);
            return true;
        } catch (Throwable e) {
            log.e("set field value " + name + " error!");
            e.printStackTrace();
            return false;
        }
    }
}
