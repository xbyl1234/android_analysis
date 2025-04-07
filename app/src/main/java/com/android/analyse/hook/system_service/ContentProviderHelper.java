package com.android.analyse.hook.system_service;

import android.os.IBinder;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.HookTools;
import com.common.tools.hooker.Hooker;

import java.util.Map;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.am.ContentProviderHelper")
public class ContentProviderHelper extends FakeClassBase {
    static ContentProviderHelper self;

    static public void Hook(ClassLoader classLoader) {
        self = new ContentProviderHelper();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    private Object getContentProviderImpl(XC_MethodHook.MethodHookParam params,
                                          @FakeParams(ClassName = "android.app.IApplicationThread") Object caller,
                                          String name, IBinder token, int callingUid, String callingPackage, String callingTag,
                                          boolean stable, int userId) throws Throwable {
        log.i("getContentProviderImpl: " + name + ", " +
                token + ", " +
                callingPackage + ", " +
                callingTag);
        if (name != null &&
                name.contains("com.findphone.ringringget.sync") ||
                name.contains("com.l58c8.qfb01b.S7227") ||
                name.contains("com.l58c8.qfb01b.sync.A1df5e")
        ) {
            log.i("pass getContentProviderImpl: " + name);
            return null;
        }
        return CallOriginalMethod(params);
    }
}
