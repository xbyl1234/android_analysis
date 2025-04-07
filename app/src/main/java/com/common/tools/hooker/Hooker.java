package com.common.tools.hooker;


import android.os.Binder;
import android.os.Build;

import com.common.log;

import java.lang.reflect.Constructor;
import java.lang.reflect.Executable;
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
        FakeMethod hookParams;

        public MethodReplacement(FakeClassBase fakeObject, Method fakeMethod, FakeMethod hookParams) throws Exception {
            this.fakeObject = fakeObject;
            this.fakeMethod = fakeMethod;
            this.fakeMethod.setAccessible(true);
            this.hookParams = hookParams;
        }

        @Override
        protected Object replaceHookedMethod(MethodHookParam param) throws Throwable {
            try {
                //static hook > fakeRunning > switch > mustHook > gid
                int callingUid = Binder.getCallingUid();
                boolean shouldFake = fakeObject.ShouldFake(param);
                if (!shouldFake && !hookParams.mustHook()) {
//                    log.d("gid:" + callingUid + ", call original method " + methodName + ", shouldFake: " + shouldFake);
                    return XposedBridge.invokeOriginalMethod(param.method, param.thisObject, param.args);
                }
//                log.d("gid:" + callingUid + ", on hook " + methodName);
                Object result = null;
                if (hookParams.hookAll()) {
                    result = fakeMethod.invoke(fakeObject, param);
                } else if (hookParams.needXposedParams()) {
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
                log.e("on hook " + fakeMethod.getName() + " error:" + targetError);
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
            boolean notFind = false;
            Method[] allMethod = fakeClass.getDeclaredMethods();
            for (Method fakeMethod : allMethod) {
                try {
                    if (fakeMethod.isAnnotationPresent(FakeMethod.class)) {
                        FakeMethod fakeMethodAnno = fakeMethod.getAnnotation(FakeMethod.class);
                        String fakeMethodName = fakeMethodAnno.methodName();
                        if (fakeMethodName.isEmpty()) {
                            fakeMethodName = fakeMethod.getName();
                        }

                        //没有这个函数,暂时不hook
                        if (!(Build.VERSION.SDK_INT >= fakeMethodAnno.minSdk() &&
                                Build.VERSION.SDK_INT <= fakeMethodAnno.maxSdk())) {
                            log.w("HookMethod " + fakeMethodName + "(" + fakeMethodAnno.minSdk() +
                                    "-" + fakeMethodAnno.maxSdk() + ") sdk level not in this system(" +
                                    Build.VERSION.SDK_INT + ")");
                            continue;
                        }
                        int paramOffset = 0;
                        if (fakeMethodAnno.needXposedParams()) {
                            paramOffset = 1;
                        }

                        StringBuilder mySignature = new StringBuilder(fakeMethodName);

                        if (fakeMethodAnno.hookAll()) {
                            MethodReplacement replacement = new MethodReplacement(fakeInst, fakeMethod, fakeMethodAnno);
                            fakeInst.fakeMethods.put(mySignature.toString(), replacement);
                            if (!fakeMethodAnno.constructor()) {
                                XposedBridge.hookAllMethods(targetClass, fakeMethodName, replacement);
                                log.d("HookAllMethod " + fakeMethodName + " success!");
                            } else {
                                XposedBridge.hookAllConstructors(targetClass, replacement);
                                log.d("HookAllConstructor " + fakeMethodName + " success!");
                            }
                        } else {
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
                            MethodReplacement replacement = new MethodReplacement(fakeInst, fakeMethod, fakeMethodAnno);
                            fakeInst.fakeMethods.put(mySignature.toString(), replacement);
                            if (!fakeMethodAnno.constructor()) {
                                Method targetMethod = XposedHelpers.findMethodExact(targetClass,
                                        fakeMethodName, paramTypes);
                                XposedBridge.hookMethod(targetMethod, replacement);
                                log.d("HookMethod " + fakeMethodName + " success!");
                            } else {
                                Constructor targetMethod = XposedHelpers.findConstructorExact(targetClass, paramTypes);
                                XposedBridge.hookMethod(targetMethod, replacement);
                                log.d("HookConstructor " + fakeMethodName + " success!");
                            }
                        }
                    }
                } catch (Throwable e) {
                    notFind = true;
                    log.e("HookMethod " + fakeMethod.getName() + " error:" + e);
                    e.printStackTrace();
                }
            }
            if (notFind) {
                Method[] methods = targetClass.getDeclaredMethods();
                for (Method m : methods) {
                    log.i("Declared " + m.toString());
                }
                methods = targetClass.getMethods();
                for (Method m : methods) {
                    log.i("Methods " + m.toString());
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
