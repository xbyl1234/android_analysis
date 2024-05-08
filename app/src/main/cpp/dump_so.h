#pragma once

#include <string>

bool dump_so(const std::string &name, const std::string &save_path_dir);

bool dump_so_delay(const string &targetLibName, int sleepTime);

bool dump_so_when_init(const string &targetLibName);