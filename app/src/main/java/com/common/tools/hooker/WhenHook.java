package com.common.tools.hooker;

import android.app.Application;
import android.content.Context;

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

    public interface WhenHookAttachCallback {
        void OnHookBefore(Application application);

        void OnHookAfter(Application application);
    }

    public interface WhenDexLoadedCallback {
        void OnHook(ClassLoader classLoader, Object[] params);
    }

    static public class PerformLaunchActivity {
        static final ArrayList<WhenHookCallback> callbacks = new ArrayList<>();
        static boolean init = false;

        public static void When(WhenHookCallback callback) throws ClassNotFoundException {
            synchronized (callbacks) {
                callbacks.add(callback);
                if (init) {
                    return;
                }
                init = true;
                XposedBridge.hookAllMethods(Class.forName("android.app.ActivityThread"), "performLaunchActivity",
                        new XC_MethodHook() {
                            @Override
                            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                                super.afterHookedMethod(param);
                                Application mInitialApplication = (Application) XposedHelpers.getObjectField(param.thisObject, "mInitialApplication");
                                ClassLoader classLoader = (ClassLoader) XposedHelpers.callMethod(mInitialApplication, "getClassLoader");
                                log.i("WhenPerformLaunchActivityHook : " + classLoader.toString());
                                for (WhenHookCallback item : callbacks) {
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
        }
    }

    static public class WhenDexLoaded {
        static final ArrayList<WhenDexLoadedCallback> callbacks = new ArrayList<>();
        static boolean init = false;

        public static void When(WhenDexLoadedCallback callback) {
            synchronized (callbacks) {
                callbacks.add(callback);
                if (init) {
                    return;
                }
                init = true;
                XposedBridge.hookAllConstructors(DexClassLoader.class, new XC_MethodHook() {
                    @Override
                    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                        super.afterHookedMethod(param);
                        log.i("WhenDexClassLoaderLoadedDex : " + param.thisObject.toString());
                        for (WhenDexLoadedCallback item : callbacks) {
                            try {
                                item.OnHook((ClassLoader) param.thisObject, param.args);
                            } catch (Throwable e) {
                                log.e("WhenDexClassLoaderLoadedDex call error: " + e);
                                e.printStackTrace();
                            }
                        }
                    }
                });
                XposedBridge.hookAllConstructors(PathClassLoader.class, new XC_MethodHook() {
                    @Override
                    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                        super.afterHookedMethod(param);
                        log.i("WhenPathClassLoaderLoadDex : " + param.thisObject.toString());
                        for (WhenDexLoadedCallback item : callbacks) {
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

    }

    static public class WhenAttach {
        static final ArrayList<WhenHookAttachCallback> callbacks = new ArrayList<>();
        static boolean init = false;

        public static void When(WhenHookAttachCallback callback) throws ClassNotFoundException {
            synchronized (callbacks) {
                callbacks.add(callback);
                if (init) {
                    return;
                }
                init = true;
                XposedHelpers.findAndHookMethod(Application.class, "attach", Context.class,
                        new XC_MethodHook() {
                            @Override
                            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                                super.beforeHookedMethod(param);
                                for (WhenHookAttachCallback item : callbacks) {
                                    try {
                                        item.OnHookBefore((Application) param.thisObject);
                                    } catch (Throwable e) {
                                        log.e("WhenAttachCallback OnHookBefore call error: " + e);
                                        e.printStackTrace();
                                    }
                                }
                            }

                            @Override
                            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                                super.afterHookedMethod(param);
                                for (WhenHookAttachCallback item : callbacks) {
                                    try {
                                        item.OnHookAfter((Application) param.thisObject);
                                    } catch (Throwable e) {
                                        log.e("WhenAttachCallback OnHookAfter error: " + e);
                                        e.printStackTrace();
                                    }
                                }
                            }
                        });
            }
        }
    }

}
