//#pragma once
//
//#include <jni.h>
//
//
//JNIEnv *get_jni_env();
//
//
////
////#ifdef __cplusplus
////extern "C" {
////#endif
////#include <jni_c.h>
////
////	JNIEnv* get_java_env();
////
////	int create_java_env(JavaVM** jvm, JNIEnv** env);
////
////	void delete_java_env(JavaVM* jvm);
////
////	int init_java();
////
////	void uninit_java();
////
////#ifdef __cplusplus
////}
////#endif
////
////
////#define JAVA_ENV get_java_env()
////#define JAVA_RENV (*get_java_env())
////
////#ifdef __cplusplus
////#include <string>
////#include <vector>
////#include <set>
////#include <map>
////#include <memory>
////using std::string;
////using std::vector;
////using std::set;
////using std::map;
////using std::shared_ptr;
////
////string jstring2str(JNIEnv* env, jstring jstr);
////
////class java_hook;
////class jclass_hook;
////
////class jmethod_hook {
////	friend class java_hook;
////	friend class jclass_hook;
////protected:
////	string name;
////	string sig;
////	jmethodID jmethod;
////	void* hook = NULL;
////
////	template<typename R, typename... Args>
////	R on_event(JNIEnv*, jobject obj, jmethodID, Args&...args) {
////		using function = R(*)(jobject obj, Args&...);
////		if (hook != NULL)
////		{
////			return ((function)hook)(obj, args...);
////		}
////		else {
////			//err
////		}
////	}
////
////public:
////
////	template<typename R, typename... Args>
////	using function = R(*)(jobject obj, Args&...);
////
////	template<typename R, typename... Args>
////	bool set_hook(function<R, Args...> pfun) {
////		hook = pfun;
////	}
////};
////
////class java_hook {
////	static map<jmethodID, shared_ptr<jmethod_hook>> hoook_list;
////	static map< int, void*> old_fun_ptr;
////private:
////	java_hook() {
////		init_hook();
////	}
////public:
////	static java_hook* get_java_hook() {
////		static java_hook* ptr = NULL;
////		if (ptr == NULL)
////		{
////			ptr = new java_hook();
////		}
////
////		return ptr;
////	}
////
////	static void set_hook(shared_ptr<jmethod_hook> hook_info) {
////		hoook_list[hook_info->jmethod] = hook_info;
////	}
////
////	template<int index, typename R>
////	static R on_call_method(JNIEnv* env, jobject obj, jmethodID jmethod, ...) {
////		using function_v = R(*)(JNIEnv* env, jobject obj, jmethodID, va_list);
////		auto item = hoook_list.find(jmethod);
////		R ret;
////
////		va_list args;
////		va_start(args, jmethod);
////
////		if (item == hoook_list.end())
////			ret = ((function_v)(old_fun_ptr.find(index + 2)->second))(env, obj, jmethod, args);
////		else
////			ret = ((function_v)(item->second->hook))(env, obj, jmethod, args);
////
////		va_end(args);
////		return ret;
////	}
////
////	template<int index, typename R, typename V>
////	static R on_call_method_(JNIEnv* env, jobject obj, jmethodID jmethod, V v) {
////		using function_v = R(*)(JNIEnv* env, jobject obj, jmethodID, V);
////		auto item = hoook_list.find(jmethod);
////		R ret;
////
////		if (item == hoook_list.end())
////			ret = ((function_v)(old_fun_ptr.find(index + 2)->second))(env, obj, jmethod, v);
////		else
////			ret = ((function_v)(item->second->hook))(env, obj, jmethod, v);
////		return ret;
////	}
////
////#define decl_call_method_hook(method)		{const int index =__COUNTER__ ;\
////											new_fun = (void*)(&java_hook::on_call_method<index, jobject>); \
////											*((void**)(&env->method)) = new_fun; \
////											old_fun_ptr[index]= (void*)env->method ;}; \
////											{const int index =__COUNTER__;\
////											new_fun = (void*)(&java_hook::on_call_method_<index, jobject,jvalue>); \
////											*((void**)(&env->method##A)) = new_fun; \
////											old_fun_ptr[index]= (void*)env->method##A;}; \
////											{const int index =__COUNTER__;\
////											new_fun = (void*)(&java_hook::on_call_method_<index, jobject,va_list>); \
////											*((void**)(&env->method##V)) = new_fun; \
////											old_fun_ptr[index]= (void*)env->method##V;};
////
////	static bool init_hook() {
////		void* new_fun;
////		JNINativeInterface_* env = (JNINativeInterface_*)JAVA_RENV;
////
////		decl_call_method_hook(CallObjectMethod);
////		decl_call_method_hook(CallBooleanMethod);
////		decl_call_method_hook(CallByteMethod);
////		decl_call_method_hook(CallCharMethod);
////		decl_call_method_hook(CallShortMethod);
////		decl_call_method_hook(CallIntMethod);
////		decl_call_method_hook(CallLongMethod);
////		decl_call_method_hook(CallFloatMethod);
////		decl_call_method_hook(CallDoubleMethod);
////		decl_call_method_hook(CallVoidMethod);
////	}
////};
////
////class jclass_hook {
////
////	string name;
////	jclass pclass;
////	vector<shared_ptr<jmethod_hook> > hoook_list;
////public:
////	jclass_hook() {
////	}
////
////	jclass_hook(string class_name) {
////		use_class(class_name);
////	}
////
////	static jclass_hook create_jclass_hook_info(string class_name) {
////		return jclass_hook(class_name);
////	}
////
////	bool use_class(string class_name) {
////		jclass pclass = JAVA_RENV->FindClass(JAVA_ENV, class_name.c_str());
////		if (pclass == NULL)
////		{
////			return false;
////		}
////
////		this->name = class_name;
////		this->pclass = pclass;
////	}
////
////	shared_ptr<jmethod_hook> use_method(const char* name, const char* sig) {
////		jmethodID jmethod = JAVA_RENV->GetMethodID(JAVA_ENV, pclass, name, sig);
////		if (jmethod == NULL)
////		{
////			return NULL;
////		}
////		shared_ptr<jmethod_hook> ptr(new jmethod_hook);
////		ptr->name = name;
////		ptr->sig = sig;
////		ptr->jmethod = jmethod;
////		hoook_list.push_back(ptr);
////		java_hook::set_hook(ptr);
////		return ptr;
////	}
////};
////
////#endif
////
////
