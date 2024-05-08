package com.common.tools.hooker;


import android.os.Binder;
import android.os.Build;

import com.common.log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;

import de.robv.android.xposed.XC_MethodReplacement;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;


public class Hooker {
    static class MethodReplacement extends XC_MethodReplacement {
        FakeClassBase fakeObject;
        Method fakeMethod;
        public Method originMethod;
        String methodName;
        boolean needXposedParams;
        boolean mustHook;

        public MethodReplacement(FakeClassBase fakeObject, Method fakeMethod, Method originMethod, FakeMethod hookParams) throws Exception {
            this.fakeObject = fakeObject;
            this.fakeMethod = fakeMethod;
            this.methodName = fakeMethod.getName();
            this.fakeMethod.setAccessible(true);
            this.originMethod = originMethod;
            this.needXposedParams = hookParams.needXposedParams();
            this.mustHook = hookParams.mustHook();
        }

        @Override
        protected Object replaceHookedMethod(MethodHookParam param) throws Throwable {
            try {
                //static hook > fakeRunning > switch > mustHook > gid
                int callingUid = Binder.getCallingUid();
                boolean shouldFake = fakeObject.ShouldFake(param);
                if (!shouldFake && !this.mustHook) {
                    log.d("gid:" + callingUid + ", call original method " + methodName + ", shouldFake: " + shouldFake);
                    return XposedBridge.invokeOriginalMethod(param.method, param.thisObject, param.args);
                }
                log.d("gid:" + callingUid + ", on hook " + methodName);
                Object result = null;
                if (needXposedParams) {
                    Object[] newArgs = new Object[param.args.length + 1];
                    System.arraycopy(param.args, 0, newArgs, 1, param.args.length);
                    newArgs[0] = param;
                    result = fakeMethod.invoke(fakeObject, newArgs);
                } else {
                    result = fakeMethod.invoke(fakeObject, param.args);
                }
                return result;
            } catch (Throwable e) {
                Throwable targetError = e;
                for (int i = 0; i < 20; i++) { // 避免死循环
                    if (targetError.getClass() == InvocationTargetException.class && targetError.getCause() != null) {
                        targetError = targetError.getCause();
                    } else {
                        break;
                    }
                }
                log.e("on hook " + methodName + " error:" + targetError);
                e.printStackTrace();
                throw targetError;
            }
        }
    }

    static public boolean HookClass(ClassLoader classLoader, FakeClassBase fakeInst) {
        try {
            Class fakeClass = fakeInst.getClass();
            if (!fakeClass.isAnnotationPresent(FakeClass.class)) {
                return false;
            }

            FakeClass fkAnno = (FakeClass) fakeClass.getDeclaredAnnotation(FakeClass.class);
            String targetClassName = fkAnno.ClassName();
            if (targetClassName == null || targetClassName.isEmpty()) {
                switch (Build.VERSION.SDK_INT) {
                    case Build.VERSION_CODES.Q:
                        targetClassName = fkAnno.ClassNameAndroid10();
                        break;
                    case Build.VERSION_CODES.P:
                        targetClassName = fkAnno.ClassNameAndroid9();
                        break;
                }
            }
            Class targetClass = XposedHelpers.findClass(targetClassName, classLoader);

            fakeInst.InitBaseFakeClass(classLoader, targetClass);

            Method[] allMethod = fakeClass.getDeclaredMethods();

            for (Method fakeMethod : allMethod) {
                try {
                    if (fakeMethod.isAnnotationPresent(FakeMethod.class)) {
                        FakeMethod anno = fakeMethod.getAnnotation(FakeMethod.class);
                        String fakeMethodName = fakeMethod.getName();

                        //没有这个函数,暂时不hook
                        if (!(Build.VERSION.SDK_INT >= anno.minSdk() && Build.VERSION.SDK_INT <= anno.maxSdk())) {
                            log.w("HookMethod " + fakeMethodName + "(" + anno.minSdk() + "-" + anno.maxSdk() +
                                    ") sdk level not in this system(" + Build.VERSION.SDK_INT + ")");
                            continue;
                        }
                        int paramOffset = 0;
                        if (anno.needXposedParams()) {
                            paramOffset = 1;
                        }

                        StringBuilder mySignature = new StringBuilder(fakeMethodName);
                        Parameter[] params = fakeMethod.getParameters();
                        Class[] paramTypes = new Class[params.length - paramOffset];
                        for (int i = paramOffset; i < params.length; i++) {
                            FakeParams fakeParams = params[i].getAnnotation(FakeParams.class);
                            if (fakeParams == null) {
                                paramTypes[i - paramOffset] = params[i].getType();
                                mySignature.append("_").append(params[i].getType().getName());
                            } else {
                                paramTypes[i - paramOffset] = XposedHelpers.findClass(fakeParams.ClassName(), classLoader);
                                mySignature.append("_").append(fakeParams.ClassName());
                            }
                        }

                        Method targetMethod = XposedHelpers.findMethodExact(targetClass,
                                fakeMethodName, paramTypes);

                        MethodReplacement replacement = new MethodReplacement(fakeInst, fakeMethod, targetMethod, anno);
                        fakeInst.fakeMethods.put(mySignature.toString(), replacement);
                        XposedBridge.hookMethod(targetMethod, replacement);
                        log.d("HookMethod " + fakeMethodName + " success!");
                    }
                } catch (Throwable e) {
                    log.e("HookMethod " + fakeMethod.getName() + " error:" + e);
                    e.printStackTrace();
                }
            }
            log.i("hook class " + targetClassName + " success!!!");
            return true;
        } catch (Throwable e) {
            log.e("hook class " + fakeInst.getClass().getName() + " failed!!!" + e);
            e.printStackTrace();
            return false;
        }
    }
}
