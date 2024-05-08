#include "hook.h"
#include "art_method_name.h"

template<class _Rp, class ..._ArgTypes>
jvalue
dealCallMethodVaList(const string &tag, const vector<Stack> &stack, _Rp(*hook)(_ArgTypes...),
                     ScopedObjectAccessAlreadyRunnable soa, jobject obj, jmethodID mid,
                     va_list args) {
    if (passCallMethod) {
        logi("pass dealCallMethodVaList");
        return hook(soa, obj, mid, args);
    }
    if (!jniTrace.CheckTargetModule(stack)) {
//        logi("call org: %p", mid);
//        for (auto item: stack) {
//            logi("--- : %s", item.name.c_str());
//        }
        return hook(soa, obj, mid, args);
    }
    callDeep++;
    defer([&]() {
        callDeep--;
    });
    if (callDeep > 1) {
        logd("pass call deep: %d", callDeep);
        return hook(soa, obj, mid, args);
    }
    try {
        if (jniTrace.CheckPassJavaMethod(mid)) {
            return hook(soa, obj, mid, args);
        }
        JNIEnv *env = *(JNIEnv **) ((uint64_t) soa + sizeof(void *));
        UnionJniEnv unionJniEnv{env, soa};

        string method_pretty_name = jniHelper.GetMethodName(mid,
                                                            method_name_type::pretty_name);
        if (jniTrace.CheckPassJavaMethod(mid, method_pretty_name)) {
            return hook(soa, obj, mid, args);
        }
        vector<string> args_type;
        string class_name, method_name, ret_type;
        if (!parse_java_method_sig(method_pretty_name, class_name, method_name, args_type,
                                   ret_type)) {
            throw "parse_java_method_sig error!";
        }
        string logs = tag + " call java: " + method_pretty_name + "\n";

        va_list va_cpy;
        va_copy(va_cpy, args);
        vector<string> argsSerialize = SerializeJavaObjectVaList(&unionJniEnv, args_type, va_cpy);
        va_end(va_cpy);

        for (int i = 0; i < args_type.size(); ++i) {
            logs += format_string("\t\t\t\t\targs %d, %s: %s\n", i, args_type[i].c_str(),
                                  argsSerialize[i].c_str());
        }
        auto result = hook(soa, obj, mid, args);
        va_end(args);

        jvalue realObject;
        bool decode = JavaSignIsObject(ret_type) && result.l != nullptr;
        if (decode) {
            realObject.l = jniHelper.AddLocalReference(env, result).l;
        } else {
            realObject = result;
        }

        string resultStr = SerializeJavaObject(&unionJniEnv, ret_type, realObject);
        logs += format_string("\t\t\t\t\tret %s: %s\n", ret_type.c_str(), resultStr.c_str());

        if (decode) {
            jniHelper.DeleteLocalRef(env, result);
        }

        string stackStr;
        for (const auto &item: stack) {
            stackStr += format_string("%p,", item.offset);
        }
        logs += format_string("\t\t\t\t\tat: %s\n", stackStr.c_str());

//        if (argsSerialize.size() == 2 && argsSerialize[1] == "1920") {
//            string unwinderStr;
//            std::vector<unwindstack::LocalFrameData> frame_info;
//            unwindstack::LocalUnwinder unwinder;
//            unwinder.Init();
//            unwinder.Unwind(&frame_info, 6);
//            for (const auto &item: frame_info) {
//                unwinderStr += format_string("%p,", item.rel_pc);
//            }
//            logi("unwinder %d - %s", unwinderStr.size(), unwinderStr.c_str());
//            logs += format_string("\t\t\t\t\tat: %s\n", unwinderStr.c_str());
//        }

        log2file("%s", logs.c_str());
        return result;
    } catch (const char *err) {
        return hook(soa, obj, mid, args);
    }
}

//_ZN3art35InvokeVirtualOrInterfaceWithVarArgsERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectP10_jmethodIDSt9__va_list
DefineHookStub(InvokeVirtualOrInterfaceWithVarArgs, jvalue,
               ScopedObjectAccessAlreadyRunnable soa, jobject obj, jmethodID mid, va_list args) {
    return dealCallMethodVaList("InvokeVirtualOrInterfaceWithVarArgs", GetStack2(),
                                pHook_InvokeVirtualOrInterfaceWithVarArgs, soa, obj, mid, args);
}

//_ZN3art17InvokeWithVarArgsERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectP10_jmethodIDSt9__va_list
DefineHookStub(InvokeWithVarArgs, jvalue,
               ScopedObjectAccessAlreadyRunnable soa, jobject obj, jmethodID mid, va_list args) {
    return dealCallMethodVaList("InvokeWithVarArgs", GetStack2(),
                                pHook_InvokeWithVarArgs, soa, obj, mid, args);
}

jobject CallStaticObjectMethod(JNIEnv *env, void *soc, jclass, jmethodID mid, ...) {
    va_list ap;
    va_start(ap, mid);
    if (soc == nullptr) {
        soc = *(void **) ((uint64_t) env + sizeof(void *));
    }
    jvalue result(pHook_InvokeWithVarArgs(soc, nullptr, mid, ap));
    jobject local_result = jniHelper.AddLocalReference(env, result).l;
    va_end(ap);
    return local_result;
}