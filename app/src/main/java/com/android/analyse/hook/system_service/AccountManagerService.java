package com.android.analyse.hook.system_service;

import android.accounts.Account;
import android.util.Log;

import com.common.log;
import com.common.tools.hooker.FakeClass;
import com.common.tools.hooker.FakeClassBase;
import com.common.tools.hooker.FakeMethod;
import com.common.tools.hooker.FakeParams;
import com.common.tools.hooker.HookTools;
import com.common.tools.hooker.Hooker;
import com.frida.frida_helper;

import java.util.List;
import java.util.concurrent.atomic.AtomicReference;

import de.robv.android.xposed.XC_MethodHook;

@FakeClass(ClassName = "com.android.server.accounts.AccountManagerService")
public class AccountManagerService extends FakeClassBase {
    static AccountManagerService self;

    static public void Hook(ClassLoader classLoader) {
        self = new AccountManagerService();
        Hooker.HookClass(classLoader, self);
    }

    @Override
    public boolean ShouldFake(XC_MethodHook.MethodHookParam params) {
        return true;
    }

    @FakeMethod(needXposedParams = true)
    List<String> getAccountRemovedReceivers(XC_MethodHook.MethodHookParam params,
                                            Account account, @FakeParams(ClassName = "com.android.server.accounts.AccountManagerService$UserAccounts") Object accounts) throws Throwable {
        List<String> result = (List<String>) CallOriginalMethod(params);
        log.i("getAccountRemovedReceivers: " + frida_helper.object_2_string(account) + ", " + frida_helper.object_2_string(result) + ", " + Log.getStackTraceString(new Exception()));
        return result;
    }

    @FakeMethod(needXposedParams = true)
    public void validateAccountsInternal(XC_MethodHook.MethodHookParam params,
                                         @FakeParams(ClassName = "com.android.server.accounts.AccountManagerService$UserAccounts") Object accounts,
                                         boolean invalidateAuthenticatorCache) throws Throwable {
        log.i("validateAccountsInternal start... " + Log.getStackTraceString(new Exception()));
        log.i("account before accountCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "accountCache")));
        log.i("account before authTokenCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "authTokenCache")));
        log.i("account before userDataCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "userDataCache")));
        log.i("account before visibilityCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "visibilityCache")));
        CallOriginalMethod(params);
        log.i("account after accountCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "accountCache")));
        log.i("account after authTokenCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "authTokenCache")));
        log.i("account after userDataCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "userDataCache")));
        log.i("account after visibilityCache:" + frida_helper.object_2_string(HookTools.GetFieldValue(accounts.getClass(), accounts, "visibilityCache")));
        log.i("validateAccountsInternal end...");
    }
}
