#pragma once

#include <string>
#include <vector>
#include <memory>
#include <regex>

#include "../third/dobby/include/dobby.h"
#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/utils.h"
#include "../third/log2file/app_file_writer.h"

using std::regex;
using std::shared_ptr;
using std::string;
using std::vector;


struct SymbolInfo {
    void *stub;
    void *target;
    void **org;
    string sym;
    bool isReg;
};

bool _resolve(SymbolInfo &item, const char *symbol, void *addr);

bool resolve(fake_dlctx_ref_t handle, vector<SymbolInfo *> *symbols);

bool hookAll(vector<SymbolInfo> *symbols);
//template<class T>
//class Call {
//private:
//    Call() {}
//};
//
//template<class _Rp, class... _ArgTypes>
//class Call<_Rp(_ArgTypes...)> : public SymbolInfo {
//public:
//    Call(string symbol) {
//    }
//
//    _Rp operator()(_ArgTypes... __arg) const {
//        return ((_Rp(*)(_ArgTypes...)) (this->orgFuc))(
//                std::forward<_ArgTypes>(__arg)...);
//    }
//};


#define ExternHookStubFunc(Func, Ret, ...) extern Ret Hook_##Func(__VA_ARGS__);
#define DefineHookStubFunc(Func, Ret, ...) Ret Hook_##Func(__VA_ARGS__)
#define DefineHookRawOrgFunc(Func, Ret, ...) Ret (*pHook_##Func)(__VA_ARGS__);
#define ExternHookRawOrgFunc(Func, Ret, ...) extern Ret (*pHook_##Func)(__VA_ARGS__);
#define DefineHookFunType(Func, Ret, ...) using Func_##Func = Ret(__VA_ARGS__);

#define DefineHookStub(Func, Ret, ...) DefineHookRawOrgFunc(Func, Ret, ##__VA_ARGS__) \
    DefineHookStubFunc(Func, Ret, ##__VA_ARGS__)

#define ExternHookStub(Func, Ret, ...)  ExternHookRawOrgFunc(Func, Ret, ##__VA_ARGS__) \
    DefineHookStubFunc(Func, Ret, ##__VA_ARGS__);


#define InlineHookAddr(Base, Addr, Func) DobbyHook((char *)Base + Addr, (dobby_dummy_func_t) & Hook_##Func, (dobby_dummy_func_t *)&pHook_##Func)

#define InlineHookSymbol(LibName, FuncName, Replace)       DobbyHook(DobbySymbolResolver(LibName,#FuncName),        \
                                                            (dobby_dummy_func_t) &Replace, (dobby_dummy_func_t *)&pHook_##FuncName)



extern void
_HookLogWitchLibWithStack(const string &libName, const string &funcName, void *ret, const char *fmt,
                          ...);

extern void setLogRetOffset(void *offset);

