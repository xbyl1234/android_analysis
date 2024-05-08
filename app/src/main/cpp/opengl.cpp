#include <jni.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#include "third/utils/log.h"
#include "base/hook.h"
#include "base/when_hook.h"
//
//vector<HookBaseInfo *> GlV1Hooks;
//vector<HookBaseInfo *> GlV2Hooks;
//vector<HookBaseInfo *> GlV3Hooks;
//
//#define DefineOpenglV1Hook(Func, Ret, ...)  _DefineHook(&GlV1Hooks, "",  #Func,Func, Ret, ##__VA_ARGS__)
//#define DefineOpenglV2Hook(Func, Ret, ...)  _DefineHook(&GlV2Hooks, "",  #Func,Func, Ret, ##__VA_ARGS__)
//#define DefineOpenglV3Hook(Func, Ret, ...)  _DefineHook(&GlV3Hooks, "",  #Func,Func, Ret, ##__VA_ARGS__)
//
//#define LOGI(...) _HookLogWitchLibWithStack
//namespace OpenglV1 {
//    DefineOpenglV1Hook (glGetString, const GLubyte *, GLenum name) {
//        logi("v1 glGetString : %p, at: %s", name, GetStack0()[0].name.c_str());
//        return hookInfo_glGetString(name);
//    }
//
//    DefineOpenglV1Hook (glGetStringi, const GLubyte *, GLenum name, GLuint index) {
//        logi("v1 glGetStringi : %p, at: %s", name, GetStack0()[0].name.c_str());
//        return hookInfo_glGetStringi(name, index);
//    }
//
//    DefineOpenglV1Hook(glGetBooleanv, void, GLenum pname, GLboolean *params) {
//        logi("v1 glGetBooleanv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetBooleanv(pname, params);
//    }
//
//    DefineOpenglV1Hook(glGetFloatv, void, GLenum pname, GLfloat *params) {
//        logi("v1 glGetFloatv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetFloatv(pname, params);
//    }
//
//    DefineOpenglV1Hook(glGetIntegerv, void, GLenum pname, GLint *params) {
//        logi("v1 glGetIntegerv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetIntegerv(pname, params);
//    }
//
//    DefineOpenglV1Hook(glGetInteger64v, void, GLenum pname, GLint64 *params) {
//        logi("v1 glGetInteger64v : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetInteger64v(pname, params);
//    }
//
//    DefineOpenglV1Hook(glGetBooleani_v, void, GLenum target, GLuint index, GLboolean *data) {
//        logi("v1 glGetBooleani_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetBooleani_v(target, index, data);
//    }
//
//    DefineOpenglV1Hook(glGetIntegeri_v, void, GLenum target, GLuint index, GLint *data) {
//        logi("v1 glGetIntegeri_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetIntegeri_v(target, index, data);
//    }
//
//    DefineOpenglV1Hook(glGetFloati_vNV, void, GLenum target, GLuint index, GLfloat *data) {
//        logi("v1 glGetFloati_vNV : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetFloati_vNV(target, index, data);
//    }
//
//    DefineOpenglV1Hook(glGetInteger64i_v, void, GLenum target, GLuint index, GLint64 *data) {
//        logi("v1 glGetInteger64i_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetInteger64i_v(target, index, data);
//    }
//}
//
//namespace OpenglV2 {
//    DefineOpenglV2Hook (glGetString, const GLubyte *, GLenum name) {
//        logi("v2 glGetString : %p, at: %s", name, GetStack0()[0].name.c_str());
//        return hookInfo_glGetString(name);
//    }
//
//    DefineOpenglV2Hook (glGetStringi, const GLubyte *, GLenum name, GLuint index) {
//        logi("v2 glGetStringi : %p, at: %s", name, GetStack0()[0].name.c_str());
//        return hookInfo_glGetStringi(name, index);
//    }
//
//    DefineOpenglV2Hook(glGetBooleanv, void, GLenum pname, GLboolean *params) {
//        logi("v2 glGetBooleanv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetBooleanv(pname, params);
//    }
//
//    DefineOpenglV2Hook(glGetFloatv, void, GLenum pname, GLfloat *params) {
//        logi("v2 glGetFloatv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetFloatv(pname, params);
//    }
//
//    DefineOpenglV2Hook(glGetIntegerv, void, GLenum pname, GLint *params) {
//        logi("v2 glGetIntegerv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetIntegerv(pname, params);
//    }
//
//    DefineOpenglV2Hook(glGetInteger64v, void, GLenum pname, GLint64 *params) {
//        logi("v2 glGetInteger64v : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetInteger64v(pname, params);
//    }
//
//    DefineOpenglV2Hook(glGetBooleani_v, void, GLenum target, GLuint index, GLboolean *data) {
//        logi("v2 glGetBooleani_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetBooleani_v(target, index, data);
//    }
//
//    DefineOpenglV2Hook(glGetIntegeri_v, void, GLenum target, GLuint index, GLint *data) {
//        logi("v2 glGetIntegeri_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetIntegeri_v(target, index, data);
//    }
//
//    DefineOpenglV2Hook(glGetFloati_vNV, void, GLenum target, GLuint index, GLfloat *data) {
//        logi("v2 glGetFloati_vNV : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetFloati_vNV(target, index, data);
//    }
//
//    DefineOpenglV2Hook(glGetInteger64i_v, void, GLenum target, GLuint index, GLint64 *data) {
//        logi("v2 glGetInteger64i_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetInteger64i_v(target, index, data);
//    }
//}
//
//namespace OpenglV3 {
//    DefineOpenglV3Hook (glGetString, const GLubyte *, GLenum name) {
//        logi("v3 glGetString : %p, at: %s", name, GetStack0()[0].name.c_str());
//        return hookInfo_glGetString(name);
//    }
//
//    DefineOpenglV3Hook (glGetStringi, const GLubyte *, GLenum name, GLuint index) {
//        logi("v3 glGetStringi : %p, at: %s", name, GetStack0()[0].name.c_str());
//        return hookInfo_glGetStringi(name, index);
//    }
//
//    DefineOpenglV3Hook(glGetBooleanv, void, GLenum pname, GLboolean *params) {
//        logi("v3 glGetBooleanv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetBooleanv(pname, params);
//    }
//
//    DefineOpenglV3Hook(glGetFloatv, void, GLenum pname, GLfloat *params) {
//        logi("v3 glGetFloatv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetFloatv(pname, params);
//    }
//
//    DefineOpenglV3Hook(glGetIntegerv, void, GLenum pname, GLint *params) {
//        logi("v3 glGetIntegerv : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetIntegerv(pname, params);
//    }
//
//    DefineOpenglV3Hook(glGetInteger64v, void, GLenum pname, GLint64 *params) {
//        logi("v3 glGetInteger64v : %p, at: %s", pname, GetStack0()[0].name.c_str());
//        hookInfo_glGetInteger64v(pname, params);
//    }
//
//    DefineOpenglV3Hook(glGetBooleani_v, void, GLenum target, GLuint index, GLboolean *data) {
//        logi("v3 glGetBooleani_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetBooleani_v(target, index, data);
//    }
//
//    DefineOpenglV3Hook(glGetIntegeri_v, void, GLenum target, GLuint index, GLint *data) {
//        logi("v3 glGetIntegeri_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetIntegeri_v(target, index, data);
//    }
//
//    DefineOpenglV3Hook(glGetFloati_vNV, void, GLenum target, GLuint index, GLfloat *data) {
//        logi("v3 glGetFloati_vNV : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetFloati_vNV(target, index, data);
//    }
//
//    DefineOpenglV3Hook(glGetInteger64i_v, void, GLenum target, GLuint index, GLint64 *data) {
//        logi("v3 glGetInteger64i_v : %p, at: %s", target, GetStack0()[0].name.c_str());
//        hookInfo_glGetInteger64i_v(target, index, data);
//    }
//}
//
//void hookOpengl(const string &libName, const vector<HookBaseInfo *> &info) {
//    auto v2 = hack_dlopen(libName.c_str(), 0);
//    SymbolInfo::resolve(v2, info);
//    for (auto item: info) {
//        if (item->is_resolved()) {
//            logi("%s hook %s", libName.c_str(), item->targetSymbol.c_str());
//            item->hook();
//        } else {
//            loge("%s cannot resolved %s", libName.c_str(), item->targetSymbol.c_str());
//        }
//    }
//    hack_dlclose(v2);
//}
//
//
//void HookOpengl() {
//
//    WhenSoInitHook("libunity.so", [](const string &path, void *addr, const string &funcType) {
//        hookOpengl("libGLESv1_CM.so", GlV1Hooks);
//        hookOpengl("libGLESv2.so", GlV2Hooks);
//        hookOpengl("libGLESv3.so", GlV3Hooks);
//    });
//
//}