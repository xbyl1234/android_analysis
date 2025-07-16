#pragma once

#include <string>
#include <mutex>
#include <string>
#include <vector>
#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include <dlfcn.h>

using std::string;
using std::mutex;
using std::vector;

struct auto_lock {
    mutex *lock;

    auto_lock(mutex *lock) {
        this->lock = lock;
        this->lock->lock();
    }

    ~auto_lock() {
        this->lock->unlock();
    }
};

#define defer_name(name, count) name##count##private
#define defer_link(class, count) defer_name(class, count)
#define defer(expr) AutoDefer defer_link(AutoDefer, __COUNTER__){ expr}


template<class T>
struct AutoDefer {
private:
    T _func = nullptr;
public:
    AutoDefer(T &&func) : _func(std::move(func)) {}

    AutoDefer(T &func) : _func(func) {}

    ~AutoDefer() {
        _func();
    }
};

struct AutoClose {
    int _fd = -1;
    FILE *file = nullptr;
private:
    AutoClose() {};
public:
    AutoClose(int fd) {
        _fd = fd;
    }

    AutoClose(FILE *fd) {
        file = fd;
    }

    ~AutoClose() {
        if (_fd != -1)
            close(_fd);
        if (file != nullptr)
            fclose(file);
    }
};


char *str2hex(const char *str, int str_len, char *hex, int buf_len);

char *hex2str(const char *hex, int hex_len, char *str, int buf_len);

vector<string> string_split(const string &str, const string &pattern);

string replace_all(const string &str, const string &old_value, const string &new_value);

string get_uuid();

int64_t get_time();

string time_to_string(int64_t tick);

int64_t string_to_time(const string &time_str, const string &fmt);

bool WritFile(const string &path, const char *buf, int len);

int gen_number(int min, int max);

float gen_double(float min, float max);

std::string gen_hexstr(int len);

std::string gen_uuid();

extern const char *CharSet_123;
extern const char *CharSet_ABC;
extern const char *CharSet_abc;
extern const char *CharSet_hex;
extern const char *CharSet_all;

std::string gen_strabc(int len);

std::string gen_strABC(int len);

std::string gen_strABC123(int len);

std::string gen_str123(int len);

string gen_strall(int len);

string gen_str(const char *charSet, int len);

std::string to_upper(const string &str);

std::string to_lower(const std::string &str);

std::string ReadFile(const std::string &path);

bool ReadFile(const std::string &path, char **data, int *len);

namespace xbyl {
    std::string format_string(const string fmt, ...);

    std::string format_string(const char *fmt, va_list ap);
}

long get_system_time_nanosecond();  // 纳秒
long get_system_time_microsecond(); // 微秒
long get_system_time_millisecond(); // 毫秒

std::string mid_string(const std::string &src, const std::string &start, const std::string &end);

int ReadPkgGid(const string &pkgName);

bool ReadAllPackagesGid(vector<int> &gids);

string ReadPkgDirSelinuxCtx(const string &pkgName);

int
TraversePackagesList(std::function<bool(const vector<string> &)> callback);

void gen_hex(int len, char *result);

string get_packet_name();

struct Stack {
    std::string name;
    void *offset;
};

vector<Stack> GetStackInfo(int num, ...);

extern "C" int get_call_stack(void *p);

bool check_mem(void *p);

extern "C" bool check_stack(void *p);

string stack2str(const vector<Stack> &stack);

extern inline std::vector<Stack> GetStackInfo() __attribute__((always_inline)) {
    std::vector<Stack> frame;
    void *p[10] = {0};
    int count = get_call_stack(p);
    for (int i = 0; i < count; i++) {
        Dl_info info{};
        void *addr = p[i];
        if (dladdr(addr, &info) != 0) {
            frame.push_back({
                                    info.dli_fname,
                                    (void *) ((uint64_t) addr - (uint64_t) info.dli_fbase)
                            });
        } else {
            frame.push_back({
                                    "unknow",
                                    (void *) ((uint64_t) addr - (uint64_t) info.dli_fbase)
                            });
        }
    }
    return frame;
}

string hexdump_memory(const uint8_t *data, size_t size, uint64_t address);

bool is_ascii_string(const uint8_t *data, size_t length);

bool safe_read_memory(uint64_t address, uint8_t *buffer, size_t length);

