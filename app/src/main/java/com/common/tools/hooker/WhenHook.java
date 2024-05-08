package com.common.tools.hooker;

import android.app.Application;

import com.common.log;

import java.util.ArrayList;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;

public class WhenHook {
    public interface WhenHookCallback {
        void OnHook(Application application, ClassLoader classLoader);
    }

    static ArrayList<WhenHookCallback> WhenPerformLaunchActivityCallback = new ArrayList<>();
    static boolean HookWhenPerformLaunchActivity = false;

    public static void WhenPerformLaunchActivityHook(WhenHookCallback callback) throws ClassNotFoundException {
        WhenPerformLaunchActivityCallback.add(callback);
        if (!HookWhenPerformLaunchActivity) {
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
    }
}
