package com.common.tools.hooker;

import android.content.Context;

import com.common.log;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;

abstract public class FakeClassSingleton extends FakeClassBase {
    public Object originObject = null;
    public Context context = null;

    public boolean OnObjectCreate(XC_MethodHook.MethodHookParam param, Object originObject) {
        return true;
    }

    @Override
    public boolean InitBaseFakeClass(ClassLoader classLoader, Class originClass) throws Throwable {
        if (!super.InitBaseFakeClass(classLoader, originClass)) {
            return false;
        }
        XposedBridge.hookAllConstructors(originClass, new XC_MethodHook() {
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                originObject = param.thisObject;
                boolean ret = OnObjectCreate(param, param.thisObject);
                log.i("on hook " + originClass.getName() + " constructors " + (ret ? "success" : "failed"));
            }
        });
        return true;
    }

    public Object CallOriginalMethod(String mySignature, Object... args) throws Throwable {
        if (originObject == null) {
            throw new HookerException("originObject is null!");
        }
        Hooker.MethodReplacement replacement = fakeMethods.get(mySignature);
        if (replacement == null) {
            throw new HookerException("replacement method is null! mySignature: " + mySignature);
        }

        return XposedBridge.invokeOriginalMethod(replacement.originMethod, originObject, args);
    }

    public Object GetFieldValue(String name) {
        return HookTools.GetFieldValue(originClass, originObject, name);
    }

    public boolean SetFieldValue(String name, Object value) {
        return HookTools.SetField(originClass, originObject, name, value);
    }
}
