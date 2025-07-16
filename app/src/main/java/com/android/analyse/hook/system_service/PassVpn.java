package com.android.analyse.hook.system_service;

import android.net.NetworkCapabilities;

import com.common.log;

import java.net.NetworkInterface;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class PassVpn {

    static public void Hook(ClassLoader classLoader) {
        XposedHelpers.findAndHookMethod(NetworkCapabilities.class, "hasTransport", int.class, new XC_MethodHook() {
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                if ((int) param.args[0] == NetworkCapabilities.TRANSPORT_VPN) {
                    log.i("pass vpn1");
                    param.setResult(false);
                }
            }
        });
        XposedHelpers.findAndHookMethod(NetworkInterface.class, "getName", new XC_MethodHook() {
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                String name = (String) param.getResult();
                if (name.contains("tun") || name.contains("ppp")) {
                    log.i("pass vpn2");
                    param.setResult("method fucker");
                }
            }
        });


    }

}
