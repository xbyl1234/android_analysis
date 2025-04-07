#pragma once

#include <fstream>
#include <string>
#include <unistd.h>

#include "../utils/jni_helper.hpp"
#include "../utils/log.h"

using namespace std;

void log2file(const string &log);

void log2file(const char *fmt, ...);

class app_file_writer {
public:
    app_file_writer(const string &dataPath, const string &name) {
        open(dataPath, name);
    }

    app_file_writer() = default;

    ~app_file_writer() {
        if (file == nullptr) {
            return;
        }
        if (file->is_open()) {
            file->close();
        }
        delete (file);
    }

    bool is_open() {
        return file != nullptr && file->is_open();
    }

    bool open(const string &dataPath, const string &name) {
        lock_guard<mutex> guard(fileLock);
        if (is_open()) {
            return true;
        }
        this->appDataPath = dataPath;
        this->fileName = name;
        this->myPid = getpid();
        file = open_file(dataPath, name);
        return file->is_open();
    }

    void write2file(const char *data, int len) {
        lock_guard<mutex> guard(fileLock);
        check_process();
        if (len > 0 && file->is_open()) {
            file->write(data, len);
            file->flush();
        }
    }

    void write2file(const string &data) {
        lock_guard<mutex> guard(fileLock);
        check_process();
        if (data.size() > 0 && file->is_open()) {
            file->write(data.c_str(), data.size());
            file->write("\n", 1);
            file->flush();
        }
    }

private:
    int myPid{};
    string appDataPath;
    string fileName;
    fstream *file{};
    mutex fileLock;

    void check_process() {
        if (myPid == getpid()) {
            return;
        }
        LOGI("log pid change: %d -> %d", myPid, getpid());
        myPid = getpid();
        file = open_file(this->appDataPath, this->fileName);
    }

public:
    static fstream *open_file(const string &appDataPath, const string &name) {
        srandom(::time(nullptr) + getpid());
        auto file = new fstream();
        string path = xbyl::format_string(appDataPath + "/%s_%d_%d", name.c_str(), getpid(),
                                    random());
        file->open(path, ios::out | ios::binary);
        if (!file->is_open()) {
            LOGI("analyse open log file %s error: %d", path.c_str(), errno);
            return file;
        }
        return file;
    }
};


