package com.android.analyse.hook.system_service;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.Hooker;

import java.io.File;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "android.content.pm.PackageParser")
public class PackageParser extends FakeClassBase {
    static PackageParser self;

    static public void Hook(ClassLoader classLoader) {
        self = new PackageParser();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod
    private static void collectCertificates(@FakeParams(ClassName = "android.content.pm.PackageParser$Package") Object pkg,
                                            File apkFile, boolean skipVerify) {
        log.i("pass collectCertificates " + pkg + ", " + apkFile.getAbsolutePath());
    }
}
