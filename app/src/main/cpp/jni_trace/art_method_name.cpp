#include "art_method_name.h"

string SymRegisterNatives;

string SymInvokeVirtualOrInterfaceWithVarArgs;
string SymInvokeWithVarArgs;

string SymNewStringUTF;
string SymGetStringUTFChars;
string SymGetStringUTFLength;

string SymGetObjectField;
string SymGetBooleanField;
string SymGetByteField;
string SymGetCharField;
string SymGetShortField;
string SymGetIntField;
string SymGetLongField;
string SymGetFloatField;
string SymGetDoubleField;

string SymSetObjectField;
string SymSetBooleanField;
string SymSetByteField;
string SymSetCharField;
string SymSetShortField;
string SymSetIntField;
string SymSetLongField;
string SymSetFloatField;
string SymSetDoubleField;

string SymGetObjectArrayElement;
string SymSetObjectArrayElement;

void initAndroid12() {
    SymInvokeVirtualOrInterfaceWithVarArgs = "_ZN3art35InvokeVirtualOrInterfaceWithVarArgsIP10_jmethodIDEENS_6JValueERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectT_St9__va_list";
    SymInvokeWithVarArgs = "_ZN3art17InvokeWithVarArgsIP10_jmethodIDEENS_6JValueERKNS_33ScopedObjectAccessAlreadyRunnableEP8_jobjectT_St9__va_list";
    SymNewStringUTF = "_ZN3art3JNIILb1EE12NewStringUTFEP7_JNIEnvPKc";
    SymGetStringUTFChars = "_ZN3art3JNIILb1EE17GetStringUTFCharsEP7_JNIEnvP8_jstringPh";
    SymGetStringUTFLength = "_ZN3art3JNIILb1EE18GetStringUTFLengthEP7_JNIEnvP8_jstring";

//    SymGetObjectField = "_ZN3art3JNIILb1EE14GetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetBooleanField = "_ZN3art3JNIILb1EE15GetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetByteField = "_ZN3art3JNIILb1EE12GetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetCharField = "_ZN3art3JNIILb1EE12GetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetShortField = "_ZN3art3JNIILb1EE13GetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetIntField = "_ZN3art3JNIILb1EE11GetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetLongField = "_ZN3art3JNIILb1EE12GetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetFloatField = "_ZN3art3JNIILb1EE13GetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
//    SymGetDoubleField = "_ZN3art3JNIILb1EE14GetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldID";

    SymGetObjectField = "_ZN3art3JNIILb0EE14GetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetBooleanField = "_ZN3art3JNIILb0EE15GetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetByteField = "_ZN3art3JNIILb0EE12GetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetCharField = "_ZN3art3JNIILb0EE12GetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetShortField = "_ZN3art3JNIILb0EE13GetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetIntField = "_ZN3art3JNIILb0EE11GetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetLongField = "_ZN3art3JNIILb0EE12GetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetFloatField = "_ZN3art3JNIILb0EE13GetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetDoubleField = "_ZN3art3JNIILb0EE14GetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldID";

    SymSetObjectField = "_ZN3art3JNIILb0EE14SetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldIDS5_";
    SymSetBooleanField = "_ZN3art3JNIILb0EE15SetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldIDh";
    SymSetByteField = "_ZN3art3JNIILb0EE12SetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldIDa";
    SymSetCharField = "_ZN3art3JNIILb0EE12SetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldIDt";
    SymSetShortField = "_ZN3art3JNIILb0EE13SetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldIDs";
    SymSetIntField = "_ZN3art3JNIILb0EE11SetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldIDi";
    SymSetLongField = "_ZN3art3JNIILb0EE12SetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldIDl";
    SymSetFloatField = "_ZN3art3JNIILb0EE13SetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldIDf";
    SymSetDoubleField = "_ZN3art3JNIILb0EE14SetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldIDd";

    SymGetObjectArrayElement = "_ZN3art3JNIILb0EE21GetObjectArrayElementEP7_JNIEnvP13_jobjectArrayi";
    SymSetObjectArrayElement = "_ZN3art3JNIILb0EE21SetObjectArrayElementEP7_JNIEnvP13_jobjectArrayiP8_jobject";
}

void initAndroid10() {
    SymInvokeVirtualOrInterfaceWithVarArgs = "InvokeVirtualOrInterfaceWithVarArgs.*__va_list$";
    SymInvokeWithVarArgs = "InvokeWithVarArgs.*__va_list$";
    SymNewStringUTF = "_ZN3art3JNI12NewStringUTFEP7_JNIEnvPKc";
    SymGetStringUTFChars = "_ZN3art3JNI17GetStringUTFCharsEP7_JNIEnvP8_jstringPh";
    SymGetStringUTFLength = "_ZN3art3JNI18GetStringUTFLengthEP7_JNIEnvP8_jstring";
    SymRegisterNatives = "_ZN3art3JNI15RegisterNativesEP7_JNIEnvP7_jclassPK15JNINativeMethodi";
    SymGetObjectField = "_ZN3art3JNI14GetObjectFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetBooleanField = "_ZN3art3JNI15GetBooleanFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetByteField = "_ZN3art3JNI12GetByteFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetCharField = "_ZN3art3JNI12GetCharFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetShortField = "_ZN3art3JNI13GetShortFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetIntField = "_ZN3art3JNI11GetIntFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetLongField = "_ZN3art3JNI12GetLongFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetFloatField = "_ZN3art3JNI13GetFloatFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
    SymGetDoubleField = "_ZN3art3JNI14GetDoubleFieldEP7_JNIEnvP8_jobjectP9_jfieldID";
};

bool initSynName() {
    switch (get_sdk_int()) {
        case 29:
            initAndroid10();
            return true;
        case 31:
        case 32:
            initAndroid12();
            return true;
    }
    return false;
}