#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <elf.h>
#include <link.h>
#include <pthread.h>
#include <sys/system_properties.h>
#include <jni.h>

#include "hack_dlopen.h"

#define nullptr 0
// the fake dlopen magic
#define FAKE_DLCTX_MAGIC (0xfaddfadd)

// http://androidxref.com/5.0.0_r2/xref/bionic/linker/dlfcn.cpp#32
#define LINKER_MUTEX "__dl__ZL10g_dl_mutex"

// the linker name
#ifndef __LP64__
#define LINKER_NAME "linker"
#else
#define LINKER_NAME "linker64"
#endif

// the jni environment on tls
__thread JNIEnv *g_tls_jnienv = nullptr;
static JavaVM *g_jvm = nullptr;
static int g_jversion = JNI_VERSION_1_4;
static pthread_mutex_t *g_linker_mutex = nullptr;

extern __attribute((weak)) int
dl_iterate_phdr(int (*)(struct dl_phdr_info *, size_t, void *), void *);

// weak symbol import
void __system_property_read_callback(
        prop_info const *info,
        void (*callback)(void *cookie, char const *name, char const *value,
                         uint32_t serial),
        void *cookie) __attribute__((weak));

int __system_property_get(char const *name, char *value) __attribute__((weak));

static void
rt_prop_read_int(void *cookie, char const *name, char const *value,
                 uint32_t serial) {
    *(int *) cookie = atoi(value);
    (void) name;
    (void) serial;
}

// read process output
static int rt_process_read(char const *cmd, char *data, size_t maxn) {
    int n = 0;
    FILE *p = popen(cmd, "r");
    if (p) {
        char buf[256] = {0};
        char *pos = data;
        char *end = data + maxn;
        while (!feof(p)) {
            if (fgets(buf, sizeof(buf), p)) {
                int len = strlen(buf);
                if (pos + len < end) {
                    memcpy(pos, buf, len);
                    pos += len;
                    n += len;
                }
            }
        }
        *pos = '\0';
        pclose(p);
    }
    return n;
}

// get system property integer
static int rt_system_property_get_int(char const *name) {

    assert_and_check_return_val(name, -1);

    int result = 0;
    if (__system_property_read_callback) {
        struct prop_info const *info = __system_property_find(name);
        if (info)
            __system_property_read_callback(info, &rt_prop_read_int, &result);
    } else if (__system_property_get) {
        char value[PROP_VALUE_MAX] = {0};
        if (__system_property_get(name, value) >= 1)
            result = atoi(value);
    } else {
        char cmd[256];
        char value[PROP_VALUE_MAX];
        snprintf(cmd, sizeof(cmd), "getprop %s", name);
        if (rt_process_read(cmd, value, sizeof(value)) > 1)
            result = atoi(value);
    }
    return result;
}

static int rt_api_level() {
    static int s_api_level = -1;
    if (s_api_level < 0)
        s_api_level = rt_system_property_get_int("ro.build.version.sdk");
    return s_api_level;
}

// find the load bias address from the base address
static void *fake_find_biasaddr_from_baseaddr(void *baseaddr) {

    assert_and_check_return_val(baseaddr, nullptr);

    // find load bias from program header
    ElfW(Ehdr) *ehdr = (ElfW(Ehdr) *) baseaddr;
    ElfW(Phdr) const *dlpi_phdr = (ElfW(Phdr) const *) (baseaddr + ehdr->e_phoff);
    int dlpi_phnum = ehdr->e_phnum;
    uintptr_t min_vaddr = UINTPTR_MAX;
    for (int i = 0; i < dlpi_phnum; i++) {
        ElfW(Phdr) const *phdr = &(dlpi_phdr[i]);
        if (PT_LOAD == phdr->p_type) {
            if (min_vaddr > phdr->p_vaddr)
                min_vaddr = phdr->p_vaddr;
        }
    }
    return min_vaddr != UINTPTR_MAX ? baseaddr - min_vaddr : nullptr;
}

// find the load bias address and real path from the maps
static void *fake_find_biasaddr_from_maps(char const *filename, char *realpath,
                                          size_t realmaxn) {

    assert_and_check_return_val(filename && realpath && realmaxn, nullptr);

    // find it
    char line[512];
    char page_attr[10];
    void *biasaddr = nullptr;
    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, filename)) {
                int pos = 0;
                uintptr_t start = 0;
                uintptr_t offset = 0;
                // 7372a68000-7372bc1000 --xp 000fe000 fd:06 39690571                       /system/lib64/libandroid_runtime.so
                if (3 == sscanf(line, "%" SCNxPTR "-%*" SCNxPTR " %4s %" SCNxPTR " %*x:%*x %*d%n",
                                &start,
                                page_attr, &offset, &pos)) {
                    if (page_attr[0] != 'r')
                        continue;
                    if (page_attr[3] != 'p')
                        continue;
                    if (0 != offset)
                        continue;

                    // get load bias address
                    biasaddr = fake_find_biasaddr_from_baseaddr((void *) start);

                    // get real path
                    if (filename[0] == '/')
                        strlcpy(realpath, filename, realmaxn);
                    else if (pos < sizeof(line)) {
                        char *p = line + pos;
                        char *e = p + strlen(p);
                        while (p < e && isspace((int) *p))
                            p++;
                        while (p < e && isspace((int) (*(e - 1))))
                            e--;
                        *e = '\0';
                        if (p < e)
                            strlcpy(realpath, p, realmaxn);
                        else
                            realpath[0] = '\0';
                    } else
                        realpath[0] = '\0';
                }
                break;
            }
        }
        fclose(fp);
    }
    return biasaddr;
}

// the callback of dl_iterate_phdr()
static int
fake_find_biasaddr_from_linker_cb(struct dl_phdr_info *info, size_t size, void *udata) {

    void const **args = (void const **) udata;
    check_return_val(args, 1);
    check_return_val(info && info->dlpi_addr && info->dlpi_name && info->dlpi_name[0] != '\0',
                     0);

    // get filename
    char const *filename = nullptr;
    char const *filepath = (char const *) args[0];
    assert_and_check_return_val(filepath, 1);
    if (filepath[0] == '/') {
        char const *p = filepath + strlen(filepath);
        while (p >= filepath && *p != '/')
            p--;
        if (p >= filepath && *p == '/')
            filename = p + 1;
    }

    // find library, we can also get full path of dlpi_name from maps
    void **pbiasaddr = (void **) &args[3];
    char *realpath = (char *) args[1];
    size_t realmaxn = (size_t) args[2];
    if ((filepath && strstr(info->dlpi_name, filepath)) ||
        (filename &&
         !strcmp(info->dlpi_name, filename))) // dlpi_name ma ybe not full path, e.g. libart.so
    {
        // save load bias address
        *pbiasaddr = (void *) info->dlpi_addr;

        // get real path
        if (filepath[0] == '/')
            strlcpy(realpath, filepath, realmaxn);
        else if (info->dlpi_name[0] == '/')
            strlcpy(realpath, info->dlpi_name, realmaxn);
        else {
            // we only find real path
            if (!fake_find_biasaddr_from_maps(filepath, realpath, realmaxn))
                realpath[0] = '\0';
        }
        // found, stop it
        return 1;
    }
    return 0;
}

// find the load bias address and real path from the maps
void *fake_find_biasaddr_from_linker(char const *filepath, char *realpath, size_t realmaxn) {
    assert_and_check_return_val(dl_iterate_phdr && filepath && realpath && realmaxn, nullptr);

    // find biasaddr
    void const *args[4];
    args[0] = (void const *) filepath;
    args[1] = (void const *) realpath;
    args[2] = (void const *) realmaxn;
    args[3] = nullptr;
    if (g_linker_mutex)
        pthread_mutex_lock(g_linker_mutex);
    dl_iterate_phdr(fake_find_biasaddr_from_linker_cb, args);
    if (g_linker_mutex)
        pthread_mutex_unlock(g_linker_mutex);
    return (void *) args[3];
}

// find the load bias address and real path
static void *
fake_find_biasaddr(char const *filename, char *realpath, size_t realmaxn) {
    assert_and_check_return_val(filename && realpath, nullptr);
    void *biasaddr = nullptr;
    if (dl_iterate_phdr && 0 != strcmp(filename, LINKER_NAME))
        biasaddr = fake_find_biasaddr_from_linker(filename, realpath, realmaxn);
    if (!biasaddr)
        biasaddr = fake_find_biasaddr_from_maps(filename, realpath, realmaxn);
    return biasaddr;
}

// open map file
static void *fake_open_file(char const *filepath, size_t *pfilesize) {

    assert_and_check_return_val(filepath && pfilesize, nullptr);

    // open it
    int fd = -1;
    void *filedata = nullptr;
    do {
        // open file
        fd = open(filepath, O_RDONLY | O_CLOEXEC);
        if (fd < 0 && errno == EINTR)
            fd = open(filepath, O_RDONLY | O_CLOEXEC);
        check_break(fd > 0);

        // get file size
        struct stat st;
        if (0 != fstat(fd, &st) || 0 == st.st_size)
            break;

        // mmap the file data
        filedata = mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        assert_and_check_break(filedata && filedata != MAP_FAILED);

        // save the file size
        if (pfilesize)
            *pfilesize = (size_t) st.st_size;

    } while (0);

    // close the fd first
    if (fd > 0)
        close(fd);
    fd = -1;

    // ok?
    return filedata;
}

static bool enum_dlsym(fake_dlctx_ref_t dlctx, enum_dlsym_callback callback, void *ctx) {
    // find the symbol address from the .dynsym first
    int i = 0;
    void *end = dlctx->filedata + dlctx->filesize;
    char const *dynstr = (char const *) dlctx->dynstr;
    ElfW(Sym) *dynsym = (ElfW(Sym) *) dlctx->dynsym;
    int dynsym_num = dlctx->dynsym_num;
    if (dynsym && dynstr) {
        for (i = 0; i < dynsym_num; i++, dynsym++) {
            char const *name = dynstr + dynsym->st_name;
            if ((void *) name < end) {
                /* NB: sym->st_value is an offset into the section for relocatables,
                 * but a VMA for shared libs or exe files, so we have to subtract the bias
                 */
                void *symboladdr = (void *) (dlctx->biasaddr + dynsym->st_value);
                if (!callback(name, symboladdr, ctx)) {
                    return false;
                }
            }
        }
    }

    // find the symbol address from the .symtab
    char const *strtab = (char const *) dlctx->strtab;
    ElfW(Sym) *symtab = (ElfW(Sym) *) dlctx->symtab;
    int symtab_num = dlctx->symtab_num;
    if (symtab && strtab) {
        for (i = 0; i < symtab_num; i++, symtab++) {
            char const *name = strtab + symtab->st_name;
            if ((void *) name < end) {
                void *symboladdr = (void *) (dlctx->biasaddr + symtab->st_value);
                if (!callback(name, symboladdr, ctx)) {
                    return false;
                }
            }
        }
    }
    return true;
}

struct find_ctx {
    void *addr;
    const char *symbol;
};

bool find_dlsym_callback(const char *symbol, void *addr, void *ctx) {
    struct find_ctx *fctx = (struct find_ctx *) ctx;
    if (strcmp(symbol, fctx->symbol) == 0) {
        fctx->addr = addr;
        return false;
    }
    return true;
}

// get symbol address from the fake dlopen context
static void *fake_dlsym(fake_dlctx_ref_t dlctx, char const *symbol) {
    assert_and_check_return_val(dlctx && dlctx->filedata && dlctx->filesize && symbol, nullptr);
    struct find_ctx ctx;
    ctx.addr = nullptr;
    ctx.symbol = symbol;
    enum_dlsym(dlctx, find_dlsym_callback, &ctx);
    return ctx.addr;
}

// close the fake dlopen context
static int fake_dlclose(fake_dlctx_ref_t dlctx) {

    assert_and_check_return_val(dlctx, -1);

    // clear data
    dlctx->biasaddr = nullptr;
    dlctx->dynsym = nullptr;
    dlctx->dynstr = nullptr;
    dlctx->dynsym_num = 0;
    dlctx->strtab = nullptr;
    dlctx->symtab = nullptr;
    dlctx->symtab_num = 0;

    // unmap file data
    if (dlctx->filedata)
        munmap(dlctx->filedata, dlctx->filesize);
    dlctx->filedata = nullptr;
    dlctx->filesize = 0;

    // free context
    free(dlctx);
    return 0;
}

/* @see https://www.sunmoonblog.com/2019/06/04/fake-dlopen/
 * https://github.com/avs333/Nougat_dlfunctions
 */
static fake_dlctx_ref_t fake_dlopen_impl(char const *filename, int flag) {

    assert_and_check_return_val(filename, nullptr);

    // do open
    bool ok = false;
    char realpath[512];
    fake_dlctx_ref_t dlctx = nullptr;
    do {
        // attempt to find the load bias address and real path
        void *biasaddr = fake_find_biasaddr(filename, realpath, sizeof(realpath));
        check_break(biasaddr);

        // init context
        dlctx = calloc(1, sizeof(fake_dlctx_t));
        assert_and_check_break(dlctx);

        dlctx->magic = FAKE_DLCTX_MAGIC;
        dlctx->biasaddr = biasaddr;

        // open file
        dlctx->filedata = fake_open_file(realpath, &dlctx->filesize);
        assert_and_check_break(dlctx->filedata && dlctx->filesize);

        // get elf
        ElfW(Ehdr) *elf = (ElfW(Ehdr) *) dlctx->filedata;
        void *end = dlctx->filedata + dlctx->filesize;
        assert_and_check_break((void *) (elf + 1) < end);

        // get .shstrtab section
        void *shoff = dlctx->filedata + elf->e_shoff;
        ElfW(Shdr) *shstrtab = (ElfW(Shdr) *) (shoff + elf->e_shstrndx * elf->e_shentsize);
        assert_and_check_break((void *) (shstrtab + 1) <= end);

        void *shstr = dlctx->filedata + shstrtab->sh_offset;
        assert_and_check_break(shstr < end);

        // parse elf sections
        int i = 0;
        bool broken = false;
        for (i = 0; !broken && i < elf->e_shnum && shoff; i++, shoff += elf->e_shentsize) {
            // get section
            ElfW(Shdr) *sh = (ElfW(Shdr) *) shoff;
            assert_and_check_break((void *) (sh + 1) <= end && shstr + sh->sh_name < end);
            assert_and_check_break(dlctx->filedata + sh->sh_offset < end);

            // get .dynsym and .symtab sections
            switch (sh->sh_type) {
                case SHT_DYNSYM:
                    // get .dynsym
                    if (dlctx->dynsym) {
                        broken = true;
                        break;
                    }
                    dlctx->dynsym = dlctx->filedata + sh->sh_offset;
                    dlctx->dynsym_num = (sh->sh_size / sizeof(ElfW(Sym)));
                    break;
                case SHT_SYMTAB:
                    // get .symtab
                    if (dlctx->symtab) {
                        broken = true;
                        break;
                    }
                    dlctx->symtab = dlctx->filedata + sh->sh_offset;
                    dlctx->symtab_num = (sh->sh_size / sizeof(ElfW(Sym)));
                    break;
                case SHT_STRTAB:
                    // get .dynstr
                    if (!strcmp(shstr + sh->sh_name, ".dynstr")) {
                        // .dynstr is guaranteed to be the first STRTAB
                        if (dlctx->dynstr)
                            break;
                        dlctx->dynstr = dlctx->filedata + sh->sh_offset;
                    }
                        // get .strtab
                    else if (!strcmp(shstr + sh->sh_name, ".strtab")) {
                        if (dlctx->strtab)
                            break;
                        dlctx->strtab = dlctx->filedata + sh->sh_offset;
                    }
                    break;
                default:
                    break;
            }
        }

        check_break(!broken && dlctx->dynstr && dlctx->dynsym);
        ok = true;
    } while (0);

    if (!ok) {
        if (dlctx != nullptr) {
            fake_dlclose(dlctx);
        }
        dlctx = nullptr;
    }
    return dlctx;
}

void hack_linker_init() {
    // we need linker mutex only for android 5.0 and 5.1
    size_t apilevel = rt_api_level();
    if (apilevel == __ANDROID_API_L__ || apilevel == __ANDROID_API_L_MR1__) {
        fake_dlctx_ref_t linker = fake_dlopen_impl(LINKER_NAME, _RTLD_NOW);
        if (linker) {
            g_linker_mutex = (pthread_mutex_t *) fake_dlsym(linker, LINKER_MUTEX);
            fake_dlclose(linker);
        }
    }
}

static fake_dlctx_ref_t fake_dlopen(char const *filename, int flag) {
    return fake_dlopen_impl(filename, flag);
}

static void jni_clearException(JNIEnv *env, bool report) {
    jthrowable e = report ? (*env)->ExceptionOccurred(env) : nullptr;
    (*env)->ExceptionClear(env);
    if (e) {
        jclass clazz = (*env)->GetObjectClass(env, e);
        jmethodID printStackTrace_id = (*env)->GetMethodID(env, clazz, "printStackTrace", "()V");
        if (!(*env)->ExceptionCheck(env) && printStackTrace_id)
            (*env)->CallVoidMethod(env, e, printStackTrace_id);
        if ((*env)->ExceptionCheck(env))
            (*env)->ExceptionClear(env);
    }
}

static jobject jni_Class_getDeclaredMethod(JNIEnv *env) {

    assert_and_check_return_val(env, nullptr);

    // push
    if ((*env)->PushLocalFrame(env, 10) < 0)
        return nullptr;

    // get unreachable memory info
    jboolean check = false;
    jobject getDeclaredMethod_method = nullptr;
    do {
        // get class
        jclass clazz = (*env)->FindClass(env, "java/lang/Class");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && clazz);

        // get string class
        jclass string_clazz = (*env)->FindClass(env, "java/lang/String");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && string_clazz);

        // get class/array class
        jclass classarray_clazz = (*env)->FindClass(env, "[Ljava/lang/Class;");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && classarray_clazz);

        // get getDeclaredMethod id
        jmethodID getDeclaredMethod_id = (*env)->GetMethodID(env, clazz, "getDeclaredMethod",
                                                             "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_id);

        // get getDeclaredMethod name
        jstring getDeclaredMethod_name = (*env)->NewStringUTF(env, "getDeclaredMethod");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_name);

        // get getDeclaredMethod args
        jobjectArray getDeclaredMethod_args = (*env)->NewObjectArray(env, 2, clazz, nullptr);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_args);

        (*env)->SetObjectArrayElement(env, getDeclaredMethod_args, 0, string_clazz);
        (*env)->SetObjectArrayElement(env, getDeclaredMethod_args, 1, classarray_clazz);

        // Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);
        getDeclaredMethod_method = (jobject) (*env)->CallObjectMethod(env, clazz,
                                                                      getDeclaredMethod_id,
                                                                      getDeclaredMethod_name,
                                                                      getDeclaredMethod_args);
        assert_and_check_break(
                !(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_method);

    } while (0);

    // exception? clear it
    if (check) {
        getDeclaredMethod_method = nullptr;
        jni_clearException(env, true);
    }
    return (jstring) (*env)->PopLocalFrame(env, getDeclaredMethod_method);
}

/* load library via system call
 *
 * @see http://weishu.me/2018/06/07/free-reflection-above-android-p/
 * https://github.com/tiann/FreeReflection/blob/c995ef100f39c2eb2d7c344384ca06e8c13b9a4c/library/src/main/java/me/weishu/reflection/Reflection.java#L23-L34
 *
 * System.load(libraryPath)
 *
 * @code
    Method forName = Class.class.getDeclaredMethod("forName", String.class);
    Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);
    Class<?> systemClass = (Class<?>)forName.invoke(null, "java.lang.System");
    Method load = (Method)getDeclaredMethod.invoke(systemClass, "load", new Class[]{String.class});
    load.invoke(systemClass, libraryPath);
 * @endcode
 *
 * System.loadLibrary(libraryName)
 *
 * @code
    Method forName = Class.class.getDeclaredMethod("forName", String.class);
    Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);
    Class<?> systemClass = (Class<?>)forName.invoke(null, "java.lang.System");
    Method loadLibrary = (Method)getDeclaredMethod.invoke(systemClass, "loadLibrary", new Class[]{String.class});
    loadLibrary.invoke(systemClass, libraryName);
 * @endcode
 */
static bool jni_System_load_or_loadLibrary_from_sys(JNIEnv *env, char const *loadName,
                                                    char const *libraryPath) {

    assert_and_check_return_val(env && loadName && libraryPath, false);

    // push
    if ((*env)->PushLocalFrame(env, 20) < 0)
        return false;

    // do load
    jboolean check = false;
    do {
        // get getDeclaredMethod method
        jobject getDeclaredMethod_method = jni_Class_getDeclaredMethod(env);
        assert_and_check_break(
                !(check = (*env)->ExceptionCheck(env)) && getDeclaredMethod_method);

        // get class
        jclass clazz = (*env)->FindClass(env, "java/lang/Class");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && clazz);

        // get object class
        jclass object_clazz = (*env)->FindClass(env, "java/lang/Object");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && object_clazz);

        // get string class
        jclass string_clazz = (*env)->FindClass(env, "java/lang/String");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && string_clazz);

        // get system class
        jclass system_clazz = (*env)->FindClass(env, "java/lang/System");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && system_clazz);

        // get method class
        jclass method_clazz = (*env)->FindClass(env, "java/lang/reflect/Method");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && method_clazz);

        // get getDeclaredMethod_method.invoke id
        jmethodID invoke_id = (*env)->GetMethodID(env, method_clazz, "invoke",
                                                  "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && invoke_id);

        // get load name
        jstring load_name = (*env)->NewStringUTF(env, loadName);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_name);

        // get invoke args
        jobjectArray invoke_args = (*env)->NewObjectArray(env, 2, object_clazz, nullptr);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && invoke_args);

        // get load args
        jobjectArray load_args = (*env)->NewObjectArray(env, 1, clazz, string_clazz);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_args);

        (*env)->SetObjectArrayElement(env, invoke_args, 0, load_name);
        (*env)->SetObjectArrayElement(env, invoke_args, 1, load_args);

        // Method load = (Method)getDeclaredMethod.invoke(systemClass, "load", new Class[]{String.class});
        jobject load_method = (jobject) (*env)->CallObjectMethod(env, getDeclaredMethod_method,
                                                                 invoke_id, system_clazz,
                                                                 invoke_args);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_method);

        // load.invoke(systemClass, libraryPath)
        jstring libraryPath_jstr = (*env)->NewStringUTF(env, libraryPath);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && libraryPath_jstr);

        invoke_args = (*env)->NewObjectArray(env, 1, object_clazz, libraryPath_jstr);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && invoke_args);

        (*env)->CallObjectMethod(env, load_method, invoke_id, system_clazz, invoke_args);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)));
    } while (0);

    // exception? clear it
    if (check)
        jni_clearException(env, true);
    (*env)->PopLocalFrame(env, nullptr);
    return !check;
}

static bool jni_System_load_or_loadLibrary_from_app(JNIEnv *env, char const *loadName,
                                                    char const *libraryPath) {

    assert_and_check_return_val(env && loadName && libraryPath, false);

    // push
    if ((*env)->PushLocalFrame(env, 10) < 0)
        return false;

    // do load
    jboolean check = false;
    do {
        // get system class
        jclass system_clazz = (*env)->FindClass(env, "java/lang/System");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && system_clazz);

        // get load/loadLibrary id
        jmethodID load_id = (*env)->GetStaticMethodID(env, system_clazz, loadName,
                                                      "(Ljava/lang/String;)V");
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && load_id);

        // get library path
        jstring libraryPath_jstr = (*env)->NewStringUTF(env, libraryPath);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)) && libraryPath_jstr);

        // load library
        (*env)->CallStaticVoidMethod(env, system_clazz, load_id, libraryPath_jstr);
        assert_and_check_break(!(check = (*env)->ExceptionCheck(env)));

    } while (0);

    // exception? clear it
    if (check)
        jni_clearException(env, true);
    (*env)->PopLocalFrame(env, nullptr);
    return !check;
}

// System.load(libraryPath)
static bool jni_System_load(JNIEnv *env, char const *libraryPath) {
    return jni_System_load_or_loadLibrary_from_app(env, "load", libraryPath) ||
           jni_System_load_or_loadLibrary_from_sys(env, "load", libraryPath);
}

// System.loadLibrary(libraryName)
static bool jni_System_loadLibrary(JNIEnv *env, char const *libraryName) {
    return jni_System_load_or_loadLibrary_from_app(env, "loadLibrary", libraryName) ||
           jni_System_load_or_loadLibrary_from_sys(env, "loadLibrary", libraryName);
}

/* get the current jni environment
 *
 * @see frameworks/base/core/jni/include/android_runtime/AndroidRuntime.h
 *
 * static AndroidRuntime* runtime = AndroidRuntime::getRuntime();
 * static JavaVM* getJavaVM() { return mJavaVM; }
 * static JNIEnv* getJNIEnv();
 */
static JNIEnv *jni_getenv() {
    if (g_jvm) {
        JNIEnv *env = nullptr;
        if (JNI_OK == (*g_jvm)->GetEnv(g_jvm, (void **) &env, g_jversion))
            return env;
    }
    if (!g_tls_jnienv) {
        fake_dlctx_ref_t dlctx = fake_dlopen("libandroid_runtime.so", _RTLD_NOW);
        if (dlctx) {
            typedef void *(*getJNIEnv_t)();
            getJNIEnv_t getJNIEnv = (getJNIEnv_t) fake_dlsym(dlctx,
                                                             "_ZN7android14AndroidRuntime9getJNIEnvEv");
            if (getJNIEnv)
                g_tls_jnienv = getJNIEnv();
            fake_dlclose(dlctx);
        }
    }
    return g_tls_jnienv;
}

void jni_javavm_set(JavaVM *jvm, int jversion) {
    g_jvm = jvm;
    g_jversion = jversion;
}

bool hack_get_module_info(const char *filename, module_info_t *info) {
    info->module_address = fake_find_biasaddr(filename, info->module_path,
                                              sizeof(info->module_path));
    return info->module_address != nullptr;
}

fake_dlctx_ref_t hack_dlopen(char const *filename, int flag) {

    assert_and_check_return_val(filename, nullptr);

    // attempt to use original dlopen to load it fist
    fake_dlctx_ref_t handle = nullptr;
    // uses the fake dlopen to load it from maps directly
    if (!handle)
        handle = fake_dlopen(filename, flag);

    // uses the fake dlopen to load it from maps directly
    if (!handle) {
        // load it via system call
        JNIEnv *env = jni_getenv();
        if (env &&
            (((strstr(filename, "/") ||
               strstr(filename, ".so")) &&
              jni_System_load(env, filename)) ||
             jni_System_loadLibrary(env, filename)))
            handle = fake_dlopen(filename, flag);
    }
    return handle;
}

void *hack_dlsym(fake_dlctx_ref_t handle, char const *symbol) {

    fake_dlctx_ref_t dlctx = (fake_dlctx_ref_t) handle;
    assert_and_check_return_val(dlctx && symbol, nullptr);

    // do dlsym
    return (dlctx->magic == FAKE_DLCTX_MAGIC) ? fake_dlsym(dlctx, symbol) : dlsym(handle, symbol);
}

bool hack_enum_dlsym(fake_dlctx_ref_t handle, enum_dlsym_callback callback, void *ctx) {
    fake_dlctx_ref_t dlctx = (fake_dlctx_ref_t) handle;
    assert_and_check_return_val(dlctx && callback, nullptr);

    return enum_dlsym(dlctx, callback, ctx);
}

int hack_dlclose(fake_dlctx_ref_t handle) {

    fake_dlctx_ref_t dlctx = (fake_dlctx_ref_t) handle;
    assert_and_check_return_val(dlctx, -1);

    // do dlclose
    return (dlctx->magic == FAKE_DLCTX_MAGIC) ? fake_dlclose(dlctx) : dlclose(handle);
}
