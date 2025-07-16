#include <jni.h>
#include <vector>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <android/log.h>
#include "third/utils/log.h"

#define CheckJni    if (env->ExceptionCheck()) { \
  env->ExceptionDescribe();                      \
  return false;}
#define CheckJni    if (env->ExceptionCheck()) { \
  env->ExceptionDescribe();                      \
  return false;}

jbyteArray createByteArray(JNIEnv *env, const char *data, int len) {
    jbyteArray result = env->NewByteArray(len);
    env->SetByteArrayRegion(result, 0, len, (const jbyte *) data);
    return result;
}

bool loadDex(JNIEnv *env, jbyteArray dexData, jobject classLoader) {
    jclass elementClass = env->FindClass("dalvik/system/DexPathList$Element");
    CheckJni
    jclass dexFileClass = env->FindClass("dalvik/system/DexFile");
    CheckJni
    jclass byteBufferClass = env->FindClass("java/nio/ByteBuffer");
    CheckJni
    jmethodID wrapMethod = env->GetStaticMethodID(byteBufferClass, "wrap",
                                                  "([B)Ljava/nio/ByteBuffer;");
    CheckJni
    jobject byteBuffer = env->CallStaticObjectMethod(byteBufferClass, wrapMethod, dexData);
    CheckJni
    jmethodID dexFileConstructor = env->GetMethodID(dexFileClass, "<init>",
                                                    "([Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;[Ldalvik/system/DexPathList$Element;)V");
    CheckJni
    jobjectArray byteBufferArray = env->NewObjectArray(1, byteBufferClass, byteBuffer);
    CheckJni
    jobject dexFile = env->NewObject(dexFileClass, dexFileConstructor, byteBufferArray, nullptr,
                                     nullptr);
    CheckJni
    jmethodID elementConstructor = env->GetMethodID(elementClass, "<init>",
                                                    "(Ldalvik/system/DexFile;)V");
    CheckJni
    jobject dexElement = env->NewObject(elementClass, elementConstructor, dexFile);
    CheckJni
    jclass baseDexClassLoaderClass = env->FindClass("dalvik/system/BaseDexClassLoader");
    CheckJni
    jfieldID pathListField = env->GetFieldID(baseDexClassLoaderClass, "pathList",
                                             "Ldalvik/system/DexPathList;");
    CheckJni
    jobject pathList = env->GetObjectField(classLoader, pathListField);
    CheckJni
    jclass dexPathListClass = env->GetObjectClass(pathList);
    CheckJni
    jfieldID dexElementsField = env->GetFieldID(dexPathListClass, "dexElements",
                                                "[Ldalvik/system/DexPathList$Element;");
    CheckJni
    auto dexElements = (jobjectArray) (env->GetObjectField(pathList, dexElementsField));
    CheckJni
    jsize dexElementsLength = env->GetArrayLength(dexElements);
    CheckJni
    jobjectArray newElements = env->NewObjectArray(dexElementsLength + 1, elementClass,
                                                   nullptr);
    CheckJni
    for (jsize i = 0; i < dexElementsLength; i++) {
        jobject element = env->GetObjectArrayElement(dexElements, i);
        env->SetObjectArrayElement(newElements, i + 1, element);
    }
    env->SetObjectArrayElement(newElements, 0, dexElement);
    CheckJni
    env->SetObjectField(pathList, dexElementsField, newElements);
    CheckJni
    return true;
}

jobject getClassLoader(JNIEnv *env, jobject obj) {
    jclass objClass = env->GetObjectClass(obj);
    if (objClass == nullptr) {
        return nullptr;
    }
    jmethodID getClassLoaderMethod = env->GetMethodID(objClass, "getClassLoader",
                                                      "()Ljava/lang/ClassLoader;");
    if (getClassLoaderMethod == nullptr) {
        return nullptr;
    }
    auto result = env->CallObjectMethod(obj, getClassLoaderMethod);
    return result;
}

jobject getApplicationRef(JNIEnv *env) {
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    if (activityThreadClass == nullptr) {
        return nullptr;
    }
    jmethodID currentApplicationMethod = env->GetStaticMethodID(activityThreadClass,
                                                                "currentApplication",
                                                                "()Landroid/app/Application;");
    if (currentApplicationMethod == nullptr) {
        return nullptr;
    }
    jobject application = env->CallStaticObjectMethod(activityThreadClass,
                                                      currentApplicationMethod);
    if (application == nullptr) {
        return nullptr;
    }
    jobject context = env->NewGlobalRef(application);
    return context;
}

jclass loadClass(JNIEnv *env, jobject classLoader, const char *clzName) {
    jclass classLoaderClass = env->GetObjectClass(classLoader);
    jmethodID loadClassMethod = env->GetMethodID(classLoaderClass, "loadClass",
                                                 "(Ljava/lang/String;)Ljava/lang/Class;");
    if (loadClassMethod == nullptr) {
        return nullptr;
    }
    jstring className = env->NewStringUTF(clzName);
    auto loadedClass = (jclass) env->CallObjectMethod(classLoader, loadClassMethod, className);
    env->DeleteLocalRef(className);
    return loadedClass;
}

jobject loadDexFromMemory(JNIEnv *env, char *dexData, int dexLen) {
    // 创建 ByteBuffer
    jclass byteBufferClass = env->FindClass("java/nio/ByteBuffer");
    jmethodID allocateDirect = env->GetStaticMethodID(byteBufferClass, "allocateDirect",
                                                      "(I)Ljava/nio/ByteBuffer;");
    jobject byteBuffer = env->CallStaticObjectMethod(byteBufferClass, allocateDirect, dexLen);
    void *bufferAddr = env->GetDirectBufferAddress(byteBuffer);
    memcpy(bufferAddr, dexData, dexLen);
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID getSystemClassLoader = env->GetStaticMethodID(classLoaderClass,
                                                            "getSystemClassLoader",
                                                            "()Ljava/lang/ClassLoader;");
    jobject systemClassLoader = env->CallStaticObjectMethod(classLoaderClass, getSystemClassLoader);
    jclass inMemoryDexClassLoaderClass = env->FindClass("dalvik/system/InMemoryDexClassLoader");
    jmethodID inMemoryDexClassLoaderInit = env->GetMethodID(inMemoryDexClassLoaderClass, "<init>",
                                                            "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
    jobject dexClassLoader = env->NewObject(inMemoryDexClassLoaderClass, inMemoryDexClassLoaderInit,
                                            byteBuffer, systemClassLoader);
    return env->NewGlobalRef(dexClassLoader);
}
//    char *data = nullptr;
//        int len;
//        if (!ReadFile("/data/frida_helper.dex", &data, &len)) {
//            logi("load dex error: %d", errno);
//            return;
//        }
//        logi("will load dex");
//        auto classLoader = loadDexFromMemory(env, data, len);
//        if (classLoader == nullptr) {
//            logi("load dex error!");
//            return;
//        }
//        jclass frida_helper = loadClass(env, classLoader, "com.frida.frida_helper");
//        if (frida_helper == nullptr) {
//            logi("frida_helper is null!");
//            return;
//        }