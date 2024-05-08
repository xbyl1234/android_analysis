package com.common.tools.proxy;

import android.os.Build;

import com.common.log;
import com.common.tools.ClassBase;
import com.common.tools.hooker.HookTools;


import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.HashMap;
import java.util.Map;

public class ProxyClass<T> extends ClassBase {
    Map<String, Method> methodMap = new HashMap<String, Method>();
    public ClassLoader classLoader = null;

    public ProxyClass(Object originObject) {
        if (originObject == null) {
            log.w("ProxyClass " + ((T) this).getClass().getName() + " originObject is null!");
        }
        this.originObject = originObject;
        this.originClass = originObject.getClass();
        this.classLoader = this.originClass.getClassLoader();
        InitMethod();
    }

    public ProxyClass(ClassLoader classLoader, String className) {
        this.originClass = HookTools.FindClass(className, classLoader);
        this.classLoader = classLoader;
        InitMethod();
    }

    public ProxyClass(ClassLoader classLoader, String className, Object originObject) {
        this.originObject = originObject;
        this.originClass = HookTools.FindClass(className, classLoader);
        this.classLoader = classLoader;
        InitMethod();
    }

    public void InitMethod() {
        Class fakeClass = ((T) this).getClass();
        Method[] allMethod = fakeClass.getDeclaredMethods();
        for (Method fakeMethod : allMethod) {
            if (!fakeMethod.isAnnotationPresent(ProxyMethod.class)) {
                continue;
            }
            ProxyMethod anno = fakeMethod.getAnnotation(ProxyMethod.class);
            String proxyMethodName = anno.methodName();
            if (proxyMethodName.isEmpty()) {
                proxyMethodName = fakeMethod.getName();
            }

            if (!(Build.VERSION.SDK_INT >= anno.minSdk() && Build.VERSION.SDK_INT <= anno.maxSdk())) {
                log.w("ProxyMethod " + proxyMethodName + "(" + anno.minSdk() + "-" + anno.maxSdk() +
                        ") sdk level not in this system(" + Build.VERSION.SDK_INT + ")");
                continue;
            }

            Parameter[] fakeParams = fakeMethod.getParameters();
            Class[] targetMethodParamsType = new Class[fakeParams.length];
            for (int i = 0; i < fakeParams.length; i++) {
                ProxyParams fakeParam = fakeParams[i].getAnnotation(ProxyParams.class);
                if (fakeParam == null) {
                    targetMethodParamsType[i] = fakeParams[i].getType();
                } else {
                    targetMethodParamsType[i] = HookTools.FindClass(fakeParam.ClassName(), classLoader);
                }
            }

            Method method = HookTools.GetMethod(originClass, proxyMethodName, targetMethodParamsType);
//            log.d("will ProxyMethod " + fakeClass.getName() + "." + proxyMethodName);
            methodMap.put(proxyMethodName, method);
        }
    }

    public Object invoke(String methodName, Object... params) throws Throwable {
        try {
            Method method = methodMap.get(methodName);
            if (method == null) {
                log.e("ProxyClass not find proxy method " + originClass.getName() + "." + methodName);
            }
            return method.invoke(originObject, params);
        } catch (InvocationTargetException e) {
            log.e("ProxyClass invoke " + originClass.getName() + "." + methodName + " error: " + e.getTargetException());
            e.getTargetException().printStackTrace();
            throw e.getTargetException();
        }
    }
}
