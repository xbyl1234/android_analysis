#pragma once

#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <time.h>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <sstream>
#include <android/log.h>
#include <jni.h>

#include "utils.h"

using std::string;


#if defined(__arm64__) || defined(__aarch64__)
#define XbylLogTagArch  "Arm64"
#elif defined(__arm__ )
#define XbylLogTagArch  "Arm32"
#elif defined(__i386__ )
#define XbylLogTagArch  "X86_32"
#elif defined(__x86_64__ )
#define XbylLogTagArch  "X86_64"
#endif

#define logd(fmt, ...) xbyl::defaultLog.do_log(xbyl::log_level::DEBUG  ,fmt, ##__VA_ARGS__)
#define logi(fmt, ...) xbyl::defaultLog.do_log(xbyl::log_level::INFO   ,fmt, ##__VA_ARGS__)
#define logw(fmt, ...) xbyl::defaultLog.do_log(xbyl::log_level::WARN   ,fmt, ##__VA_ARGS__)
#define loge(fmt, ...) xbyl::defaultLog.do_log(xbyl::log_level::ERROR  ,fmt, ##__VA_ARGS__)

#define XbylLogTag "analyse_log"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, XbylLogTag, __VA_ARGS__)

namespace xbyl {
    using func_log = void (*)(const string &msg);

    using std::mutex;

    static string level_string[]{
            "Unknown",
            "Default",
            "Verbose",
            "Debug",
            "Info",
            "Warn",
            "Error",
            "Fatal",
            "Silent",
    };

    enum class log_adapt {
        use_file,
        use_printf,
        use_custom_func,
        use_adb,
        use_none,
        use_client,
    };

    enum class log_level {
        UNKNOWN = ANDROID_LOG_UNKNOWN,
        DEFAULT = ANDROID_LOG_DEFAULT,
        VERBOSE = ANDROID_LOG_VERBOSE,
        DEBUG = ANDROID_LOG_DEBUG,
        INFO = ANDROID_LOG_INFO,
        WARN = ANDROID_LOG_WARN,
        ERROR = ANDROID_LOG_ERROR,
        FATAL = ANDROID_LOG_FATAL,
        SILENT = ANDROID_LOG_SILENT,
    };


    class adapter {
    public:
        std::mutex lock;
        log_adapt type;
        bool enabled = true;

        virtual void output_log(log_level level, const string &buf) = 0;
    };

    class adapter_file : public adapter {
        FILE *log_file;

    public:
        adapter_file(const string &path) {
            type = log_adapt::use_file;
            log_file = fopen(path.c_str(), "w+");
            if (log_file == nullptr) {
                LOGI("log open file %s error: %d", path.c_str(), errno);
            }
        }

        ~adapter_file() {
            if (log_file) {
                fclose(log_file);
            }
            log_file = nullptr;
        }

        virtual void output_log(log_level, const string &buf) {
            lock.lock();
            fwrite(buf.c_str(), buf.size(), 1, log_file);
            fflush(log_file);
            lock.unlock();
        }
    };

    class adapter_printf : public adapter {
    public:
        adapter_printf() {
            type = log_adapt::use_printf;
        }

        virtual void output_log(log_level, const string &buf) {
            printf("%s\n", buf.c_str());
        }
    };

    class adapter_adb : public adapter {
    public:
        adapter_adb() {
            type = log_adapt::use_adb;
        }

        virtual void output_log(log_level level, const string &buf) {
            __android_log_write((int) level, XbylLogTag, buf.c_str());
        }
    };

    class adapter_custom : public adapter {
        func_log flog;
    public:
        adapter_custom() {
            type = log_adapt::use_client;
        }

        virtual void output_log(log_level level, const string &buf) {

        }
    };

    class log {
    public:
        vector<std::unique_ptr<adapter>> adapters;
        string tag = XbylLogTag "-Native" XbylLogTagArch "-";
        bool enabled = true;

        log() {
            adapters.push_back(std::make_unique<adapter_adb>());
        }

        void setTag(const string &_tag) {
            tag = XbylLogTag "-Native" XbylLogTagArch "-";
            tag += _tag;
        }

        ~log() {
        }

        bool set_adapt(adapter *adapt) {
            adapters.push_back(std::unique_ptr<adapter>(adapt));
            return true;
        }

        void do_log(log_level level, const char *fmt, ...) {
            if (adapters.size() == 0 || !enabled) {
                return;
            }

            string head;
            head += level_string[(int) level];
            head += "\t";
            head += tag;
            head += ":\t\t\t";

            va_list ap;
            va_start(ap, fmt);
            string result = format_string((head + fmt).c_str(), ap);
            va_end(ap);
            for (const std::unique_ptr<adapter> &item: adapters) {
                if (item->enabled) {
                    item->output_log(level, result);
                }
            }
        }
    };

    extern log defaultLog;

    void enable_adb_log();

    void disable_adb_log();

    void init_log(const string &tag);

    void init_log(const string &tag, adapter *adapt);

//    void init_log(const string &tag, vector<std::unique_ptr<adapter>> &adapt);
}

