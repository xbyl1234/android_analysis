package com.android.analyse.hook.system_service;

import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.HookTools;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.am.ProcessList")
public class ProcessList extends FakeClassBase {
    static ProcessList self;

    static public void Hook(ClassLoader classLoader) {
        self = new ProcessList();
        Hooker.HookClass(classLoader, self);
    }

    @FakeMethod(needXposedParams = true)
    private Object startProcess(XC_MethodHook.MethodHookParam params,
                                @FakeParams(ClassName = "com.android.server.am.HostingRecord") Object hostingRecord,
                                String entryPoint, @FakeParams(ClassName = "com.android.server.am.ProcessRecord")
                                Object app, int uid, int[] gids, int runtimeFlags, int zygotePolicyFlags,
                                int mountExternal, String seInfo, String requiredAbi, String instructionSet,
                                String invokeWith, long startTime) throws Throwable {
        log.i("startProcess " + entryPoint + ", " +
                frida_helper.object_2_string(hostingRecord) + ", " +
                Log.getStackTraceString(new Throwable()));
//        String mDefiningPackageName = (String) HookTools.GetFieldValue(hostingRecord.getClass(), hostingRecord, "mDefiningPackageName");
//        String mDefiningProcessName = (String) HookTools.GetFieldValue(hostingRecord.getClass(), hostingRecord, "mDefiningProcessName");
//        String mHostingName = (String) HookTools.GetFieldValue(hostingRecord.getClass(), hostingRecord, "mHostingName");
//        String mHostingType = (String) HookTools.GetFieldValue(hostingRecord.getClass(), hostingRecord, "mHostingType");
//        if (mHostingName != null && (
//                mHostingName.contains("com.bdddd.heartlove/com.l58c8.qfb01b.sync.Q997") ||
//                        mHostingName.contains("com.l58c8.qfb01b.sync.Q997") ||
//                        mHostingName.contains("com.l58c8.qfb01b.sync.Y0b60") ||
//                        mHostingName.contains("com.l58c8.qfb01b.sync.A1df5e") ||
//                        mHostingName.contains("com.w2a7.ybfdc77.V201529") ||
//                        mHostingName.contains("Scf4c3") ||
//                        mHostingName.contains("com.oaa9.a0d10a9.Y06") ||
//                        mHostingName.contains("com.y8f.le6.Abecfb") ||
//                        mHostingName.contains("com.l58c8.qfb01b.S7227") ||
//                        mHostingName.contains("com.findphone.ringringget.sync")
//        )) {
//            log.i("pass startProcess " + mHostingName);
//            throw new RuntimeException();
//        }
        return CallOriginalMethod(params);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }
}
