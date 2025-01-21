package com.common.tools.hooker;

import android.app.Application;

import com.common.log;

import java.util.ArrayList;

import dalvik.system.DexClassLoader;
import dalvik.system.PathClassLoader;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;

public class WhenHook {
    public interface WhenHookCallback {
        void OnHook(Application application, ClassLoader classLoader);
    }

    public interface WhenDexLoadedCallback {
        void OnHook(ClassLoader classLoader, Object[] params);
    }
    static ArrayList<WhenHookCallback> WhenPerformLaunchActivityCallback = new ArrayList<>();
    static boolean HookWhenPerformLaunchActivity = false;

    static ArrayList<WhenDexLoadedCallback> WhenDexClassLoaderLoadedDexCallback = new ArrayList<>();
    static boolean HookWhenDexClassLoaderLoadedDex = false;
    public static void WhenPerformLaunchActivityHook(WhenHookCallback callback) throws ClassNotFoundException {
        WhenPerformLaunchActivityCallback.add(callback);
        if (HookWhenPerformLaunchActivity) {
            return;
        }
        HookWhenPerformLaunchActivity = true;
            XposedBridge.hookAllMethods(Class.forName("android.app.ActivityThread"), "performLaunchActivity",
                    new XC_MethodHook() {
                        @Override
                        protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                            super.afterHookedMethod(param);
                            Application mInitialApplication = (Application) XposedHelpers.getObjectField(param.thisObject, "mInitialApplication");
                            ClassLoader classLoader = (ClassLoader) XposedHelpers.callMethod(mInitialApplication, "getClassLoader");
                            log.i("WhenPerformLaunchActivityHook : " + classLoader.toString());
                            for (WhenHookCallback item : WhenPerformLaunchActivityCallback) {
                                try {
                                    item.OnHook(mInitialApplication, classLoader);
                                } catch (Throwable e) {
                                    log.e("WhenPerformLaunchActivityHook call error: " + e);
                                    e.printStackTrace();
                                }
                            }
                        }
                    });
        }
    static ArrayList<WhenDexLoadedCallback> WhenPathClassLoaderLoadDexCallback = new ArrayList<>();
    static boolean HookWhenPathClassLoaderLoadDex = false;
    public static void WhenPathClassLoaderLoadDex(WhenDexLoadedCallback callback) {
        synchronized (WhenPathClassLoaderLoadDexCallback) {
            WhenPathClassLoaderLoadDexCallback.add(callback);
            if (HookWhenPathClassLoaderLoadDex) {
                return;
            }
            HookWhenPathClassLoaderLoadDex = true;
            XposedBridge.hookAllConstructors(PathClassLoader.class, new XC_MethodHook() {
                @Override
                protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                    super.afterHookedMethod(param);
                    log.i("WhenPathClassLoaderLoadDex : " + param.thisObject.toString());
                    for (WhenDexLoadedCallback item : WhenPathClassLoaderLoadDexCallback) {
                        try {
                            item.OnHook((ClassLoader) param.thisObject, param.args);
                        } catch (Throwable e) {
                            log.e("WhenPathClassLoaderLoadDex call error: " + e);
                            e.printStackTrace();
                        }
                    }
                }
            });
        }
    }
    public static void WhenDexClassLoaderLoadedDex(WhenDexLoadedCallback callback) {
        synchronized (WhenDexClassLoaderLoadedDexCallback) {
            WhenDexClassLoaderLoadedDexCallback.add(callback);
            if (HookWhenDexClassLoaderLoadedDex) {
                return;
            }
            HookWhenDexClassLoaderLoadedDex = true;
            XposedBridge.hookAllConstructors(DexClassLoader.class, new XC_MethodHook() {
                @Override
                protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                    super.afterHookedMethod(param);
                    log.i("WhenDexClassLoaderLoadedDex : " + param.thisObject.toString());
                    for (WhenDexLoadedCallback item : WhenDexClassLoaderLoadedDexCallback) {
                        try {
                            item.OnHook((ClassLoader) param.thisObject, param.args);
                        } catch (Throwable e) {
                            log.e("WhenDexClassLoaderLoadedDex call error: " + e);
                            e.printStackTrace();
                        }
                    }
                }
            });
        }
    }
}
