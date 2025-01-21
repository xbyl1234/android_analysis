#include <map>
#include <string>
#include "../third/utils/jni_helper.hpp"
#include "art_method_name.h"

using namespace std;
static map<string, string> android10;
static map<string, string> android12;
static map<string, string> android13;

map<string, string> initAndroid13(JNIEnv *env) {
    android13 = map<string, string>{
            {"InvokeVirtualOrInterfaceWithVarArgs", "InvokeVirtualOrInterfaceWithVarArgs.*__va_list$"},
            {"InvokeWithVarArgs",                   "InvokeWithVarArgs.*__va_list$"},
            {"NewStringUTF",                        "NewStringUTF"},
            {"GetStringUTFChars",                   "GetStringUTFChars"},
            {"GetStringUTFLength",                  "GetStringUTFLength"},
            {"RegisterNatives",                     "RegisterNatives"},
            {"GetObjectField",                      "GetObjectField"},
            {"GetBooleanField",                     "GetBooleanField"},
            {"GetByteField",                        "GetByteField"},
            {"GetCharField",                        "GetCharField"},
            {"GetShortField",                       "GetShortField"},
            {"GetIntField",                         "GetIntField"},
            {"GetLongField",                        "GetLongField"},
            {"GetFloatField",                       "GetFloatField"},
            {"GetDoubleField",                      "GetDoubleField"},
            {"SetObjectField",                      "SetObjectField"},
            {"SetBooleanField",                     "SetBooleanField"},
            {"SetByteField",                        "SetByteField"},
            {"SetCharField",                        "SetCharField"},
            {"SetShortField",                       "SetShortField"},
            {"SetIntField",                         "SetIntField"},
            {"SetLongField",                        "SetLongField"},
            {"SetFloatField",                       "SetFloatField"},
            {"SetDoubleField",                      "SetDoubleField"},
            {"GetObjectArrayElement",               "GetObjectArrayElement"},
            {"SetObjectArrayElement",               "SetObjectArrayElement"},
    };
    return android13;
}

map<string, string> initAndroid12(JNIEnv *env) {
    android12 = map<string, string>{
            {"InvokeVirtualOrInterfaceWithVarArgs", "_ZN3art35InvokeVirtualOrInterfaceWithVarArgsIP10_jmethodIDEENS_6JValueERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectT_St9__va_list"},
            {"InvokeWithVarArgs",                   "_ZN3art17InvokeWithVarArgsIP10_jmethodIDEENS_6JValueERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectT_St9__va_list"},
            {"NewStringUTF",                        "_ZN3art3JNIILb0EE12NewStringUTFEP7_JNIEnvPKc"},
            {"GetStringUTFChars",                   "_ZN3art3JNIILb0EE17GetStringUTFCharsEP7_JNIEnvP8_jstringPh"},
            {"GetStringUTFLength",                  "_ZN3art3JNIILb0EE18GetStringUTFLengthEP7_JNIEnvP8_jstring"},
            {"RegisterNatives",                     "_ZN3art3JNIILb0EE15RegisterNativesEP7_JNIEnvP7_jclassPK15JNINativeMethodi"},
            {"GetObjectField",                      "_ZN3art3JNIILb0EE14GetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetBooleanField",                     "_ZN3art3JNIILb0EE15GetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetByteField",                        "_ZN3art3JNIILb0EE12GetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetCharField",                        "_ZN3art3JNIILb0EE12GetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetShortField",                       "_ZN3art3JNIILb0EE13GetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetIntField",                         "_ZN3art3JNIILb0EE11GetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetLongField",                        "_ZN3art3JNIILb0EE12GetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetFloatField",                       "_ZN3art3JNIILb0EE13GetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetDoubleField",                      "_ZN3art3JNIILb0EE14GetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"SetObjectField",                      "_ZN3art3JNIILb0EE14SetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldIDS5_"},
            {"SetBooleanField",                     "_ZN3art3JNIILb0EE15SetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldIDh"},
            {"SetByteField",                        "_ZN3art3JNIILb0EE12SetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldIDa"},
            {"SetCharField",                        "_ZN3art3JNIILb0EE12SetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldIDt"},
            {"SetShortField",                       "_ZN3art3JNIILb0EE13SetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldIDs"},
            {"SetIntField",                         "_ZN3art3JNIILb0EE11SetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldIDi"},
            {"SetLongField",                        "_ZN3art3JNIILb0EE12SetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldIDl"},
            {"SetFloatField",                       "_ZN3art3JNIILb0EE13SetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldIDf"},
            {"SetDoubleField",                      "_ZN3art3JNIILb0EE14SetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldIDd"},
            {"GetObjectArrayElement",               "_ZN3art3JNIILb0EE21GetObjectArrayElementEP7_JNIEnvP13_jobjectArrayi"},
            {"SetObjectArrayElement",               "_ZN3art3JNIILb0EE21SetObjectArrayElementEP7_JNIEnvP13_jobjectArrayiP8_jobject"},
    };
    return android12;
}

map<string, string> initAndroid10(JNIEnv *env) {
    android10 = map<string, string>{
            {"InvokeVirtualOrInterfaceWithVarArgs", "InvokeVirtualOrInterfaceWithVarArgs.*__va_list$"},
            {"InvokeWithVarArgs",                   "InvokeWithVarArgs.*__va_list$"},
            {"NewStringUTF",                        "_ZN3art3JNI12NewStringUTFEP7_JNIEnvPKc"},
            {"GetStringUTFChars",                   "_ZN3art3JNI17GetStringUTFCharsEP7_JNIEnvP8_jstringPh"},
            {"GetStringUTFLength",                  "_ZN3art3JNI18GetStringUTFLengthEP7_JNIEnvP8_jstring"},
            {"RegisterNatives",                     "_ZN3art3JNI15RegisterNativesEP7_JNIEnvP7_jclassPK15JNINativeMethodi"},
            {"GetObjectField",                      "_ZN3art3JNI14GetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetBooleanField",                     "_ZN3art3JNI15GetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetByteField",                        "_ZN3art3JNI12GetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetCharField",                        "_ZN3art3JNI12GetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetShortField",                       "_ZN3art3JNI13GetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetIntField",                         "_ZN3art3JNI11GetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetLongField",                        "_ZN3art3JNI12GetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetFloatField",                       "_ZN3art3JNI13GetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"GetDoubleField",                      "_ZN3art3JNI14GetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldID"},
            {"SetObjectField",                      "_ZN3art3JNI14SetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldIDS4_"},
            {"SetBooleanField",                     "_ZN3art3JNI15SetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldIDh"},
            {"SetByteField",                        "_ZN3art3JNI12SetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldIDa"},
            {"SetCharField",                        "_ZN3art3JNI12SetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldIDt"},
            {"SetShortField",                       "_ZN3art3JNI13SetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldIDs"},
            {"SetIntField",                         "_ZN3art3JNI11SetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldIDi"},
            {"SetLongField",                        "_ZN3art3JNI12SetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldIDl"},
            {"SetFloatField",                       "_ZN3art3JNI13SetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldIDf"},
            {"SetDoubleField",                      "_ZN3art3JNI14SetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldIDd"},
            {"GetObjectArrayElement",               "_ZN3art3JNI21GetObjectArrayElementEP7_JNIEnvP13_jobjectArrayi"},
            {"SetObjectArrayElement",               "_ZN3art3JNI21SetObjectArrayElementEP7_JNIEnvP13_jobjectArrayiP8_jobject"},
    };
    return android10;
}

map<string, string> getSynName(JNIEnv *env) {
    switch (get_sdk_int()) {
        case 29:
            return initAndroid10(env);
        case 31:
        case 32:
            return initAndroid12(env);
        case 33:
            return initAndroid13(env);
    }
    return map<string, string>();
}