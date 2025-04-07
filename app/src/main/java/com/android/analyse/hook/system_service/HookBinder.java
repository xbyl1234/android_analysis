package com.android.analyse.hook.system_service;

import android.os.Binder;
import android.os.Parcel;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.HookTools;
import com.common.tools.hooker.Hooker;

import java.util.HashMap;
import java.util.Map;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;
//
//@FakeClass(ClassName = "android.os.Binder")
//public class HookBinder extends FakeClassBase {
//    static class BinderConfig {
//        public String tags;
//        public Map<Integer, String> transactName;
//    }
//
//    static HookBinder self;
//    static Map<Binder, BinderConfig> binderMap;
//
//    static public void Hook(ClassLoader classLoader) throws Throwable {
//        self = new HookBinder();
//        binderMap = new HashMap<>();
//        Hooker.HookClass(classLoader, self);
//
//
//        log.i("binderMap put bind " + param.args[0]);
//        BinderConfig config = new BinderConfig();
//        config.tags = "IPackageManager";
//        config.transactName = HookTools.getTransactName("android.content.pm.IPackageManager$Stub");
//        binderMap.put((Binder) param.args[0], config);
//        XposedHelpers.findAndHookConstructor("android.content.pm.IPackageManager$Stub$Proxy",
//                classLoader, android.os.IBinder.class, new XC_MethodHook() {
//                    @Override
//                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
//                        super.beforeHookedMethod(param);
//
//                    }
//                });
//    }
//
//    @Override
//    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
//        return true;
//    }
//
//    @FakeMethod(needXposedParams = true)
//    boolean onTransact(XC_MethodHook.MethodHookParam params, int code, Parcel data, Parcel reply, int flags) throws Throwable {
//        BinderConfig config = binderMap.get(params.thisObject);
//        if (config != null) {
//            log.i(config.tags + " onTransact: " + config.transactName.get(code));
//        }
//        return (boolean) CallOriginalMethod(params);
//    }
//}
