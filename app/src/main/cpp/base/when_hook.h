#pragma once

#include <string>
#include <functional>

using WhenHookCallBack = std::function<void(const string &path, void *addr,
                                            const string &funcType)>;

bool WhenSoInitHook(const std::string &soName, WhenHookCallBack _callBack);

