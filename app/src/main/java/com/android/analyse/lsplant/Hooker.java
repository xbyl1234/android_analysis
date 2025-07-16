package com.android.analyse.lsplant;

import com.common.log;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class Hooker {
    public static abstract class Callback {
        public Method backup;
        private Method target;

        native Method hook(Method target, Method callback);

        public abstract Object onCall(Object thisObj, Object[] params) throws Throwable;

        public Object call(Object thisObj, Object[] params) throws Throwable {
            return backup.invoke(thisObj, params);
        }

        public Object callback(Object[] args) throws Throwable {
            Object thisObj = args[0];
            Object[] params = new Object[args.length - 1];
            System.arraycopy(args, 1, params, 0, args.length - 1);
            return onCall(thisObj, params);
        }
    }

    static Class findClass(String name, ClassLoader classLoader) {
        try {
            return Class.forName(name, false, classLoader);
        } catch (ClassNotFoundException e) {
            log.e("not find " + name);
            return null;
        }
    }

    static Method findMethod(Class clz, String name, Class... params) {
        try {
            return clz.getDeclaredMethod(name, params);
        } catch (NoSuchMethodException e) {
            log.e("not find " + name);
            return null;
        }
    }

    static native boolean init();

    native boolean unhook(Method target);

    public boolean unhook(Callback callback) {
        return unhook(callback.target);
    }


    public static boolean SetField(Field field, Object obj, Object value) throws Throwable {
        boolean oldAcc = field.isAccessible();
        field.setAccessible(true);
        field.set(obj, value);
        field.setAccessible(oldAcc);
        return true;
    }

    public static boolean SetField(Object obj, String name, Object value) throws Throwable {
        Field field = GetField(obj.getClass(), name);
        if (field == null) {
            return false;
        }
        boolean oldAcc = field.isAccessible();
        field.setAccessible(true);
        field.set(obj, value);
        field.setAccessible(oldAcc);
        return true;
    }

    public static boolean SetField(Class _class, Object obj, String name, Object value) throws Throwable {
        Field field = GetField(_class, name);
        boolean oldAcc = field.isAccessible();
        field.setAccessible(true);
        field.set(obj, value);
        field.setAccessible(oldAcc);
        return true;
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

    public static Hooker hook(Method target, Callback callback) throws Throwable {
        Hooker hooker = new Hooker();
        Method callbackMethod = Callback.class.getDeclaredMethod("callback", Object[].class);
        Method result = callback.hook(target, callbackMethod);
        if (result == null) {
            throw new Exception("hook failed");
        }
        callback.backup = result;
        callback.target = target;
        return hooker;
    }
}
