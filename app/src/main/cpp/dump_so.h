#pragma once

#include <string>
#include <functional>

bool dump_so(const std::string &name, const std::string &save_path_dir);

bool dump_so_delay(const string &targetLibName, int sleepTime);

bool dump_so_when_init(const string &targetLibName);

using callback = std::function<void(void *addr, int size, int prot, const string &name)>;

void dump_mprotect(callback cb);