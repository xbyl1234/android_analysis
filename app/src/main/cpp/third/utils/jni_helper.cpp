#include "jni_helper.hpp"

jstring char2jstring(JNIEnv *env, const char *pat) {
    //定义java String类 strClass
    jclass strClass = (env)->FindClass("java/lang/String");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
    //将byte数组转换为java String,并输出
    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
}

jstring string2jstring(JNIEnv *env, const string &str) {
    return char2jstring(env, str.c_str());
}

string jstring2string(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    auto barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen == 0) {
        env->ReleaseByteArrayElements(barr, ba, 0);
        return string();
    }
    rtn = (char *) malloc(alen + 1);
    memcpy(rtn, ba, alen);
    rtn[alen] = 0;
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

vector<jobject> get_java_list(JNIEnv *env, jobject obj_list) {
    jclass list_cls = env->GetObjectClass(obj_list);
    jmethodID list_get = env->GetMethodID(list_cls, "get", "(I)Ljava/lang/Object;");
    jmethodID list_size = env->GetMethodID(list_cls, "size", "()I");
    int len = env->CallIntMethod(obj_list, list_size);
    vector<jobject> ret;
    for (int i = 0; i < len; i++) {
        auto element = (jstring) (env->CallObjectMethod(obj_list, list_get, i));
        if (element == nullptr) {
            continue;
        }
        ret.push_back(element);
        env->DeleteLocalRef(element);
    }
    return ret;
}




//struct auto_init_jni_helper {
//    auto_init_jni_helper() {
//        auto handle = fake_dlopen("libart.so", 0);
//        if (handle == nullptr) {
//            loge("fake_dlopen libart.so is null!", "");
//            return;
//        }
//        _ZN3art9ArtMethod12JniShortNameEv = (func_type1) fake_dlsym(handle,
//                                                                    "_ZN3art9ArtMethod12JniShortNameEv");
//        if (_ZN3art9ArtMethod12JniShortNameEv == nullptr) {
//            loge("fake_dlsym _ZN3art9ArtMethod12JniShortNameEv is null!", "");
//        }
//        _ZN3art9ArtMethod11JniLongNameEv = (func_type1) fake_dlsym(handle,
//                                                                   "_ZN3art9ArtMethod11JniLongNameEv");
//        if (_ZN3art9ArtMethod11JniLongNameEv == nullptr) {
//            loge("fake_dlsym _ZN3art9ArtMethod11JniLongNameEv is null!", "");
//        }
//        _ZN3art9ArtMethod12PrettyMethodEb = (func_type2) fake_dlsym(handle,
//                                                                    "_ZN3art9ArtMethod12PrettyMethodEb");
//        if (_ZN3art9ArtMethod12PrettyMethodEb == nullptr) {
//            loge("fake_dlsym _ZN3art9ArtMethod12PrettyMethodEb is null!", "");
//        }
//        fake_dlclose(handle);
//    }
//} init_jni_help;



string get_object_class_name(JNIEnv *env, jobject obj) {
//"class [[I"
    jclass jc = env->GetObjectClass(obj);
    jmethodID toString = env->GetMethodID(jc, "toString",
                                          "()Ljava/lang/String;");
//    if (env->ExceptionCheck()) {
//        env->ExceptionDescribe();
//        env->ExceptionClear();
//    }
    string ret = jstring2string(env, (jstring) env->CallObjectMethod(jc, toString));
    return ret.substr(ret.find(" ") + 1);
}

jthrowable clean_exception(JNIEnv *env) {
    if (env->ExceptionCheck()) {
        jthrowable t;
        t = env->ExceptionOccurred();
        env->ExceptionDescribe();
        env->ExceptionClear();
        return t;
    }
    return nullptr;
}

jobject get_application(JNIEnv *env) {
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID get_Application = env->GetStaticMethodID(activity_thread_clz,
                                                           "currentActivityThread",
                                                           "()Landroid/app/ActivityThread;");
        if (get_Application != NULL) {
            jobject currentActivityThread = env->CallStaticObjectMethod(activity_thread_clz,
                                                                        get_Application);
            jmethodID getal = env->GetMethodID(activity_thread_clz, "getApplication",
                                               "()Landroid/app/Application;");
            application = env->CallObjectMethod(currentActivityThread, getal);
        }
        return application;
    }
    return application;
}

jstring get_package_name(JNIEnv *env) {
    jobject context = get_application(env);
    if (context == NULL) {
        return NULL;
    }
    jclass activity = env->GetObjectClass(context);
    jmethodID methodId_pack = env->GetMethodID(activity, "getPackageName", "()Ljava/lang/String;");
    jstring name_str = static_cast<jstring >( env->CallObjectMethod(context, methodId_pack));
    return name_str;
}

int get_sdk_int() {
    char buff[256] = {0};
    __system_property_get("ro.build.version.sdk", buff);
    return atoi(buff);
}

string prop_get_string(const string &key) {
    char buff[256] = {0};
    __system_property_get(key.c_str(), buff);
    return buff;
}

