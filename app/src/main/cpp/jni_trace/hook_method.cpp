#include "hook.h"
#include "art_method_name.h"
#include "log_maker.h"

#define IvkCallA(name, result, env, obj, method, jv)  \
    Logs logs;                                        \
    logs.setStack(MethodStack());                      \
    logs.setJniEnv(env);                              \
    logs.setName(#name);                              \
    logs.setCallParams(nullptr, obj, method, jv);     \
    result r = pHook_##name(env, obj, method, jv);    \
    logs.setCallResult((uint64_t)r);                      \
    logs.log();                                       \
    return r;

#define IvkCallAVoid(name, result, env, obj, method, jv)\
    Logs logs;                                          \
    logs.setStack(MethodStack());                        \
    logs.setJniEnv(env);                                \
    logs.setName(#name);                                \
    logs.setCallParams(nullptr, obj, method, jv);       \
    pHook_##name(env, obj, method, jv);                 \
    logs.log();

#define IvkCallAClz(name, result, env, obj, clz, method, jv)\
    Logs logs;                                              \
    logs.setStack(MethodStack());                            \
    logs.setJniEnv(env);                                    \
    logs.setName(#name);                                    \
    logs.setCallParams(clz, obj,  method, jv);              \
    result r = pHook_##name(env, obj,clz, method, jv);      \
    logs.setCallResult((uint64_t)r);                            \
    logs.log();                                             \
    return r;

#define IvkCallAClzVoid(name, result, env, obj, clz, method, jv) \
    Logs logs;                                                   \
    logs.setStack(MethodStack());                                 \
    logs.setJniEnv(env);                                         \
    logs.setName(#name);                                         \
    logs.setCallParams(clz, obj,  method, jv);                   \
    pHook_##name(env, obj,clz, method, jv);                      \
    logs.log();

#define IvkCall(name, result, env, obj, method)       \
    va_list ap;                                       \
    va_start(ap, method);                             \
    Logs logs;                                        \
    logs.setStack(MethodStack());                      \
    logs.setJniEnv(env);                              \
    logs.setName(#name);                              \
    logs.setCallParams(nullptr, obj,  method, ap);    \
    result r = pHook_##name##V(env, obj, method, ap); \
    logs.setCallResult((uint64_t)r);                      \
    va_end(ap);                                       \
    logs.log();                                       \
    return r;

#define IvkCallVoid(name, result, env, obj, method) \
    va_list ap;                                     \
    va_start(ap, method);                           \
    Logs logs;                                      \
    logs.setStack(MethodStack());                    \
    logs.setJniEnv(env);                            \
    logs.setName(#name);                            \
    logs.setCallParams(nullptr, obj,  method, ap);  \
    pHook_##name##V(env, obj, method, ap);          \
    va_end(ap);                                     \
    logs.log();

#define IvkCallClz(name, result, env, obj, clz, method)  \
    va_list ap;                                          \
    va_start(ap, method);                                \
    Logs logs;                                           \
    logs.setStack(MethodStack());                         \
    logs.setJniEnv(env);                                 \
    logs.setName(#name);                                 \
    logs.setCallParams(clz, obj,  method, ap);           \
    result r = pHook_##name##V(env, obj,clz, method, ap);\
    logs.setCallResult((uint64_t)r);                         \
    va_end(ap);                                          \
    logs.log();                                          \
    return r;

#define IvkCallVoidClz(name, result, env, obj, clz, method) \
    va_list ap;                                             \
    va_start(ap, method);                                   \
    Logs logs;                                              \
    logs.setStack(MethodStack());                            \
    logs.setJniEnv(env);                                    \
    logs.setName(#name);                                    \
    logs.setCallParams(clz, obj,  method, ap);              \
    pHook_##name##V(env, obj, clz,method, ap);              \
    va_end(ap);                                             \
    logs.log();

DefineHookStub(CallObjectMethodA, jobject, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) {
    IvkCallA(CallObjectMethodA, jobject, env, obj, method, jv)
}

DefineHookStub(CallBooleanMethodA, jboolean, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) {
    IvkCallA(CallBooleanMethodA, jboolean, env, obj, method, jv)
}

DefineHookStub(CallByteMethodA, jbyte, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallA(CallByteMethodA, jbyte, env, obj, method, jv) }

DefineHookStub(CallCharMethodA, jchar, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallA(CallCharMethodA, jchar, env, obj, method, jv) }

DefineHookStub(CallShortMethodA, jshort, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallA(CallShortMethodA, jshort, env, obj, method, jv) }

DefineHookStub(CallIntMethodA, jint, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallA(CallIntMethodA, jint, env, obj, method, jv) }

DefineHookStub(CallLongMethodA, jlong, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallA(CallLongMethodA, jlong, env, obj, method, jv) }

DefineHookStub(CallFloatMethodA, jfloat, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallA(CallFloatMethodA, jfloat, env, obj, method, jv) }

DefineHookStub(CallDoubleMethodA, jdouble, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallA(CallDoubleMethodA, jdouble, env, obj, method, jv) }

DefineHookStub(CallVoidMethodA, void, JNIEnv *env, jobject obj, jmethodID method,
               const jvalue *jv) { IvkCallAVoid(CallVoidMethodA, void, env, obj, method, jv); }

DefineHookStub(CallNonvirtualObjectMethodA, jobject, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualObjectMethodA, jobject, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualBooleanMethodA, jboolean, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualBooleanMethodA, jboolean, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualByteMethodA, jbyte, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualByteMethodA, jbyte, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualCharMethodA, jchar, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualCharMethodA, jchar, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualShortMethodA, jshort, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualShortMethodA, jshort, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualIntMethodA, jint, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualIntMethodA, jint, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualLongMethodA, jlong, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualLongMethodA, jlong, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualFloatMethodA, jfloat, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualFloatMethodA, jfloat, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualDoubleMethodA, jdouble, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClz(CallNonvirtualDoubleMethodA, jdouble, env, obj, clz, method, jv);
}

DefineHookStub(CallNonvirtualVoidMethodA, void, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, const jvalue *jv) {
    IvkCallAClzVoid(CallNonvirtualVoidMethodA, void, env, obj, clz, method, jv);
}

DefineHookStub(CallObjectMethodV, jobject, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallObjectMethodV, jobject, env, obj, method, va); }

DefineHookStub(CallBooleanMethodV, jboolean, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallBooleanMethodV, jboolean, env, obj, method, va); }

DefineHookStub(CallByteMethodV, jbyte, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallByteMethodV, jbyte, env, obj, method, va); }

DefineHookStub(CallCharMethodV, jchar, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallCharMethodV, jchar, env, obj, method, va); }

DefineHookStub(CallShortMethodV, jshort, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallShortMethodV, jshort, env, obj, method, va); }

DefineHookStub(CallIntMethodV, jint, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallIntMethodV, jint, env, obj, method, va); }

DefineHookStub(CallLongMethodV, jlong, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallLongMethodV, jlong, env, obj, method, va); }

DefineHookStub(CallFloatMethodV, jfloat, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallFloatMethodV, jfloat, env, obj, method, va); }

DefineHookStub(CallDoubleMethodV, jdouble, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallA(CallDoubleMethodV, jdouble, env, obj, method, va); }

DefineHookStub(CallVoidMethodV, void, JNIEnv *env, jobject obj, jmethodID method,
               va_list va) { IvkCallAVoid(CallVoidMethodV, void, env, obj, method, va); }

DefineHookStub(CallNonvirtualObjectMethodV, jobject, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualObjectMethodV, jobject, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualBooleanMethodV, jboolean, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualBooleanMethodV, jboolean, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualByteMethodV, jbyte, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualByteMethodV, jbyte, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualCharMethodV, jchar, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualCharMethodV, jchar, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualShortMethodV, jshort, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualShortMethodV, jshort, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualIntMethodV, jint, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualIntMethodV, jint, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualLongMethodV, jlong, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualLongMethodV, jlong, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualFloatMethodV, jfloat, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualFloatMethodV, jfloat, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualDoubleMethodV, jdouble, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClz(CallNonvirtualDoubleMethodV, jdouble, env, obj, clz, method, va);
}

DefineHookStub(CallNonvirtualVoidMethodV, void, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, va_list va) {
    IvkCallAClzVoid(CallNonvirtualVoidMethodV, void, env, obj, clz, method, va);
}

DefineHookStub(CallObjectMethod, jobject, JNIEnv *env, jobject obj, jmethodID method,
               ...) { IvkCall(CallObjectMethod, jobject, env, obj, method); }

DefineHookStub(CallBooleanMethod, jboolean, JNIEnv *env, jobject obj, jmethodID method,
               ...) { IvkCall(CallBooleanMethod, jboolean, env, obj, method); }

DefineHookStub(CallByteMethod, jbyte, JNIEnv *env, jobject obj, jmethodID method, ...) {
    IvkCall(CallByteMethod, jbyte, env, obj, method);
}

DefineHookStub(CallCharMethod, jchar, JNIEnv *env, jobject obj, jmethodID method, ...) {
    IvkCall(CallCharMethod, jchar, env, obj, method);
}

DefineHookStub(CallShortMethod, jshort, JNIEnv *env, jobject obj, jmethodID method, ...) {
    IvkCall(CallShortMethod, jshort, env, obj, method);
}

DefineHookStub(CallIntMethod, jint, JNIEnv *env, jobject obj, jmethodID method, ...) {
    IvkCall(CallIntMethod, jint, env, obj, method);
}

DefineHookStub(CallLongMethod, jlong, JNIEnv *env, jobject obj, jmethodID method, ...) {
    IvkCall(CallLongMethod, jlong, env, obj, method);
}

DefineHookStub(CallFloatMethod, jfloat, JNIEnv *env, jobject obj, jmethodID method, ...) {
    IvkCall(CallFloatMethod, jfloat, env, obj, method);
}

DefineHookStub(CallDoubleMethod, jdouble, JNIEnv *env, jobject obj, jmethodID method,
               ...) { IvkCall(CallDoubleMethod, jdouble, env, obj, method); }

DefineHookStub(CallVoidMethod, void, JNIEnv *env, jobject obj, jmethodID method, ...) {
    IvkCallVoid(CallVoidMethod, void, env, obj, method);
}

DefineHookStub(CallNonvirtualObjectMethod, jobject, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualObjectMethod, jobject, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualBooleanMethod, jboolean, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualBooleanMethod, jboolean, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualByteMethod, jbyte, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualByteMethod, jbyte, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualCharMethod, jchar, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualCharMethod, jchar, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualShortMethod, jshort, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualShortMethod, jshort, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualIntMethod, jint, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualIntMethod, jint, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualLongMethod, jlong, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualLongMethod, jlong, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualFloatMethod, jfloat, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualFloatMethod, jfloat, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualDoubleMethod, jdouble, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallClz(CallNonvirtualDoubleMethod, jdouble, env, obj, clz, method);
}

DefineHookStub(CallNonvirtualVoidMethod, void, JNIEnv *env, jobject obj, jclass clz,
               jmethodID method, ...) {
    IvkCallVoidClz(CallNonvirtualVoidMethod, void, env, obj, clz, method);
}



//template<class _Rp, class ..._ArgTypes>
//jvalue
//dealCallMethodVaList(const string &tag, const vector<Stack> &stack, _Rp(*hook)(_ArgTypes...),
//                     ScopedObjectAccessAlreadyRunnable soa, jobject obj, jmethodID mid,
//                     va_list args) {
//    if (passCallMethod) {
////        logi("pass dealCallMethodVaList");
//        return hook(soa, obj, mid, args);
//    }
//    if (!jniTrace.CheckTargetModule(stack) || passJniTrace) {
//        return hook(soa, obj, mid, args);
//    }
//    passJniTrace = true;
//    defer([] { passJniTrace = false; });
//
//    try {
//        if (jniTrace.CheckPassJavaMethod(mid)) {
//            return hook(soa, obj, mid, args);
//        }
//        JNIEnv *env = *(JNIEnv **) ((uint64_t) soa + sizeof(void *));
//
//        string method_pretty_name = jniHelper.GetMethodName(mid,
//                                                            method_name_type::pretty_name);
//        if (jniTrace.CheckPassJavaMethod(mid, method_pretty_name)) {
//            return hook(soa, obj, mid, args);
//        }
//        vector<string> args_type;
//        string class_name, method_name, ret_type;
//        if (!parse_java_method_sig(method_pretty_name, class_name, method_name, args_type,
//                                   ret_type)) {
//            throw "parse_java_method_sig error!";
//        }
//        string logs = tag + " call java: " + method_pretty_name + "\n";
//
//        va_list va_cpy;
//        va_copy(va_cpy, args);
//        vector<string> argsSerialize = SerializeJavaObjectVaList(env, args_type, va_cpy);
//        va_end(va_cpy);
//
//        for (int i = 0; i < args_type.size(); ++i) {
//            logs += xbyl::format_string("\t\t\t\t\targs %d, %s: %s\n", i, args_type[i].c_str(),
//                                  argsSerialize[i].c_str());
//        }
//        auto result = hook(soa, obj, mid, args);
//        va_end(args);
//
//        jvalue realObject;
//        bool decode = JavaSignIsObject(ret_type) && result.l != nullptr;
//        if (decode) {
//            realObject.l = jniHelper.AddLocalReference(env, result).l;
//        } else {
//            realObject = result;
//        }
//
//        string resultStr = SerializeJavaObject(env, ret_type, realObject);
//        logs += xbyl::format_string("\t\t\t\t\tret %s: %s\n", ret_type.c_str(), resultStr.c_str());
//
//        if (decode) {
//            jniHelper.DeleteLocalRef(env, result);
//        }
//
//        string stackStr;
//        for (const auto &item: stack) {
//            stackStr += xbyl::format_string("%p,", item.offset);
//        }
//        logs += xbyl::format_string("\t\t\t\t\tat: %s\n", stackStr.c_str());
//
////        if (argsSerialize.size() == 2 && argsSerialize[1] == "1920") {
////            string unwinderStr;
////            std::vector<unwindstack::LocalFrameData> frame_info;
////            unwindstack::LocalUnwinder unwinder;
////            unwinder.Init();
////            unwinder.Unwind(&frame_info, 6);
////            for (const auto &item: frame_info) {
////                unwinderStr += xbyl::format_string("%p,", item.rel_pc);
////            }
////            logi("unwinder %d - %s", unwinderStr.size(), unwinderStr.c_str());
////            logs += xbyl::format_string("\t\t\t\t\tat: %s\n", unwinderStr.c_str());
////        }
//
//        log2file("%s", logs.c_str());
//        return result;
//    } catch (const char *err) {
//        return hook(soa, obj, mid, args);
//    }
//}
//
////_ZN3art35InvokeVirtualOrInterfaceWithVarArgsERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectP10_jmethodIDSt9__va_list
//DefineHookStub(InvokeVirtualOrInterfaceWithVarArgs, jvalue,
//               ScopedObjectAccessAlreadyRunnable soa, jobject obj, jmethodID mid, va_list args) {
//    return dealCallMethodVaList("InvokeVirtualOrInterfaceWithVarArgs", GetStack01(),
//                                pHook_InvokeVirtualOrInterfaceWithVarArgs, soa, obj, mid, args);
//}
//
////_ZN3art17InvokeWithVarArgsERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectP10_jmethodIDSt9__va_list
//DefineHookStub(InvokeWithVarArgs, jvalue,
//               ScopedObjectAccessAlreadyRunnable soa, jobject obj, jmethodID mid, va_list args) {
//    return dealCallMethodVaList("InvokeWithVarArgs", GetStack01(),
//                                pHook_InvokeWithVarArgs, soa, obj, mid, args);
//}
//jobject CallStaticObjectMethod(JNIEnv *env, void *soc, jclass, jmethodID mid, ...) {
//    va_list ap;
//    va_start(ap, mid);
//    if (soc == nullptr) {
//        soc = *(void **) ((uint64_t) env + sizeof(void *));
//    }
//    jvalue result(pHook_InvokeWithVarArgs(soc, nullptr, mid, ap));
//    jobject local_result = jniHelper.AddLocalReference(env, result).l;
//    va_end(ap);
//    return local_result;
//}
