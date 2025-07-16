package com.android.analyse.hook.system_service;

import static de.robv.android.xposed.XposedHelpers.findAndHookMethod;

import java.lang.reflect.Field;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class FakeGAid {

    static public void Hook(ClassLoader classLoader) {

        XposedHelpers.findAndHookMethod("com.google.android.gms.ads.identifier.internal.IAdvertisingIdService$Stub",
                classLoader, "getAdvertisingIdInfo", new XC_MethodHook() {
                    @Override
                    protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                        Object result = param.getResult();
                        Class<?> clazz = result.getClass();

                        Field idField = clazz.getDeclaredField("advertisingId");
                        idField.setAccessible(true);
                        idField.set(result, "YOUR_NEW_GAID");

                        Field limitField = clazz.getDeclaredField("limitAdTrackingEnabled");
                        limitField.setAccessible(true);
                        limitField.set(result, false);
                    }
                });
    }
}
