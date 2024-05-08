package com.frida;


import com.alibaba.fastjson2.JSONArray;
import com.alibaba.fastjson2.JSONObject;

import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

public class Reflect {

    interface ObjectReflect {
        Object Reflect(Object obj);
    }

    static public Map<Pattern, ObjectReflect> objectReflect = new HashMap<>();

    static {
        ObjectReflect doNotReflect = new ObjectReflect() {
            @Override
            public Object Reflect(Object obj) {
                return obj.toString();
            }
        };
        objectReflect.put(Pattern.compile("X.00H"), doNotReflect);
//        objectReflect.put(Pattern.compile("X.0K0"), doNotReflect);
        objectReflect.put(Pattern.compile("com.facebook.lite.LeanClientApplication"), doNotReflect);
        objectReflect.put(Pattern.compile("android.*"), doNotReflect);
        objectReflect.put(Pattern.compile("androidx.lifecycle.*"), doNotReflect);
        objectReflect.put(Pattern.compile("dalvik.*"), doNotReflect);
        objectReflect.put(Pattern.compile("libcore.io.*"), doNotReflect);
        objectReflect.put(Pattern.compile("sun.misc.Cleaner"), doNotReflect);
        objectReflect.put(Pattern.compile("java.lang.ClassLoader"), doNotReflect);
        objectReflect.put(Pattern.compile("java.lang.Package"), doNotReflect);
        objectReflect.put(Pattern.compile("java.lang.File"), doNotReflect);
        objectReflect.put(Pattern.compile("java.util.jar.JarFile"), doNotReflect);
    }

    static public ObjectReflect GetCustomTypeReflect(Class cls) {
        for (Pattern item : objectReflect.keySet()) {
            if (item.matcher(cls.getName()).find()) {
                return objectReflect.get(item);
            }
        }
        return null;
    }

    public static Field GetField(String _className, String name) throws Throwable {
        Class _class = Class.forName(_className);
        Field field = _class.getField(name);
        field.setAccessible(true);
        return field;
    }

    public static Field GetField(Class _class, String name) {
        try {
            return _class.getDeclaredField(name);
        } catch (NoSuchFieldException e) {
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
        } catch (IllegalAccessException e) {
            log.e(e.toString());
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

    static class ReflectJson {
        boolean canPrivate;
        boolean canStatic;
        Set<String> dup = new HashSet<>();

        public ReflectJson(boolean canPrivate, boolean canStatic) {
            this.canPrivate = canPrivate;
            this.canStatic = canStatic;
        }

        public boolean isBaseType(Class cls) {
            if (cls == null) {
                return true;
            }
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

        public boolean isBaseArrayType(Class cls) {
            if (cls.isArray()) {
                return isBaseType(cls.getComponentType());
            }
            return false;
//            if (Collection.class.isAssignableFrom(cls)) {
//                return isBaseType(getArrayType(obj));
//            }
//            if (isMap(cls)) {
//                return isBaseType(getMapValueType(obj));
//            }
        }

        public boolean isArray(Class clz) {
            return clz.isArray() || Collection.class.isAssignableFrom(clz);
        }

        public boolean isMap(Class clz) {
            return Map.class.isAssignableFrom(clz);
        }

        public Class getMapValueType(Object obj) {
            if (obj == null) {
                return null;
            }
            if (((Map) obj).values().size() == 0) {
                return null;
            }
            for (Object v : ((Map) obj).values()) {
                if (v != null) {
                    return v.getClass();
                }
            }
            return null;
        }

        public Class getArrayType(Object obj) {
            if (obj == null) {
                return null;
            }
            Collection c = ((Collection) obj);
            if (c.size() == 0) {
                return null;
            }
            for (Object o : c) {
                if (o != null) {
                    return o.getClass();
                }
            }
            return null;
        }

        public Class getArrayType(Field mapField) {
            try {
                Type mapMainType = mapField.getGenericType();
                if (mapMainType instanceof ParameterizedType) {
                    ParameterizedType parameterizedType = (ParameterizedType) mapMainType;
                    Type[] types = parameterizedType.getActualTypeArguments();
                    if (types.length == 1) {
                        return types[0].getClass();
                    }
                }
                return null;
            } catch (Throwable e) {
                return null;
            }
        }

        public Class getCollItemType(Class cls) {
            Class<?> clazz = List.class; // 假设是 List<Integer> 类型
            Type genericType = clazz.getGenericSuperclass();

            if (genericType instanceof ParameterizedType) {
                ParameterizedType parameterizedType = (ParameterizedType) genericType;
                Type[] typeArguments = parameterizedType.getActualTypeArguments();
                if (typeArguments.length > 0) {
                    Type elementType = typeArguments[0];
                    return elementType.getClass();
                }
            }
            return null;
        }

        public static Class getSuperClass(Class<?> calzz) {
            Class<?> superclass = calzz.getSuperclass();
            if (superclass == Object.class) {
                return null;
            }
            return superclass;
        }

        public Object ReflectGetFields(Class clz, Object target, int deep) {
            if (target == null) {
                return null;
            }
            if (clz == null) {
                clz = target.getClass();
            }
            if (dup.contains("" + System.identityHashCode(target) + System.identityHashCode(clz))) {
                return "dup obj!" + target;
            }
            dup.add("" + System.identityHashCode(target) + System.identityHashCode(clz));
            log.i(target.getClass() + "  " + System.identityHashCode(target) + "  " + System.identityHashCode(clz));
            if (deep - 1 == 0) {
                return target;
            }
            if (isBaseType(clz) || isBaseArrayType(clz)) {
                return target;
            }
            if (isArray(clz)) {
                return ReflectGetFieldsArray(clz, target, deep - 1);
            }
            if (isMap(clz)) {
                return ReflectGetFieldsMap(target, deep - 1);
            }
            return ReflectGetFieldsObj(clz, target, deep - 1);
        }

        public Object ReflectGetFieldsArray(Class clz, Object target, int deep) {
            if (target == null) {
                return target;
            }
            JSONArray jas = new JSONArray();
            if (Collection.class.isAssignableFrom(clz)) {
                Collection c = (Collection) target;
                for (Object obj : c) {
                    jas.add(ReflectGetFields(null, obj, deep - 1));
                }
            } else if (clz.isArray()) {
                for (int i = 0; i < Array.getLength(target); i++) {
                    Object obj = Array.get(target, i);
                    jas.add(ReflectGetFields(null, obj, deep - 1));
                }
            }
            return jas;
        }

        public Object ReflectGetFieldsMap(Object target, int deep) {
            if (target == null) {
                return target;
            }

            JSONObject json = new JSONObject();
            Map map = (Map) target;
            for (Object key : map.keySet()) {
                Object value = map.get(key);
                json.put(key.toString(), ReflectGetFields(null, value, deep - 1));
            }
            return json;
        }

        public JSONObject ReflectGetFieldsObj(Class _class, Object target, int deep) {
            JSONObject json = new JSONObject();
            ObjectReflect or = GetCustomTypeReflect(_class);
            if (or != null) {
                json.put("_self", or.Reflect(target));
                return json;
            }
            Field[] fields = _class.getDeclaredFields();
            for (Field field : fields) {
                try {
                    field.setAccessible(true);
                    if (!canStatic && Modifier.isStatic(field.getModifiers())) {
                        continue;
                    }
                    if (!canPrivate && !Modifier.isPublic(field.getModifiers())) {
                        continue;
                    }

                    Object obj = field.get(target);
                    Object putValue;
                    if (obj == null || isBaseType(obj.getClass())) {
                        putValue = obj;
                    } else {
                        ObjectReflect or2 = GetCustomTypeReflect(obj.getClass());
                        if (or2 != null) {
                            putValue = or2.Reflect(obj);
                        } else if (deep - 1 != 0) {
                            putValue = ReflectGetFields(obj.getClass(), obj, deep - 1);
                        } else {
                            putValue = obj;
                        }
                    }

                    if (Modifier.isStatic(field.getModifiers())) {
                        json.put(field.getName(), putValue);
                    } else if (Modifier.isPublic(field.getModifiers())) {
                        json.put(field.getName(), putValue);
                    } else {
                        json.put(field.getName(), putValue);
                    }
                } catch (IllegalAccessException e) {
                    json.put(field.getName(), e);
                }
            }

            Class sup = getSuperClass(_class);
            if (sup != null) {
                ObjectReflect orsup = GetCustomTypeReflect(sup);
                if (orsup != null) {
                    json.put("_super", orsup.Reflect(target));
                } else {
                    json.put("_super", ReflectGetFields(sup, target, deep));
                }
            }
            return json;
        }
    }

    public static Object ReflectGetFields(Class _class, Object target, boolean canPrivate, boolean canStatic) {
        ReflectJson json = new ReflectJson(canPrivate, canStatic);
        return json.ReflectGetFields(_class, target, -1);
    }

    public static Object ReflectGetFields(Class _class, boolean canPrivate) {
        ReflectJson json = new ReflectJson(canPrivate, true);
        return json.ReflectGetFieldsObj(_class, null, 5);
    }
    public static Object ReflectGetPublicNoStatic(Object target) {
        return ReflectGetFields(target, false, false);
    }

    public static Object ReflectGetNoStatic(Object target) {
        return ReflectGetFields(target, true, false);
    }

    public static Object ReflectGetFields(Object target, boolean canPrivate, boolean canStatic) {
        if (target == null) {
            return null;
        }
        return ReflectGetFields(target.getClass(), target, canPrivate, canStatic);
    }
}
