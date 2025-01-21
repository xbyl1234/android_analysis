#pragma once

#include "../base/hook.h"

class jni_sym {
public:
    vector<SymbolInfo> jniHooks;

    bool init(fake_dlctx_ref_t handleLibArt, JNIEnv *env);

};