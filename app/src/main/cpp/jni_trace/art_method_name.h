#pragma once

#include <string>
#include "../third/utils/jni_helper.hpp"

using namespace std;

extern string SymRegisterNatives;

extern string SymInvokeVirtualOrInterfaceWithVarArgs;
extern string SymInvokeWithVarArgs;

extern string SymNewStringUTF;
extern string SymGetStringUTFChars;
extern string SymGetStringUTFLength;

extern string SymGetObjectField;
extern string SymGetBooleanField;
extern string SymGetByteField;
extern string SymGetCharField;
extern string SymGetShortField;
extern string SymGetIntField;
extern string SymGetLongField;
extern string SymGetFloatField;
extern string SymGetDoubleField;

extern string SymSetObjectField;
extern string SymSetBooleanField;
extern string SymSetByteField;
extern string SymSetCharField;
extern string SymSetShortField;
extern string SymSetIntField;
extern string SymSetLongField;
extern string SymSetFloatField;
extern string SymSetDoubleField;

extern string SymGetObjectArrayElement;
extern string SymSetObjectArrayElement;

bool initSynName();