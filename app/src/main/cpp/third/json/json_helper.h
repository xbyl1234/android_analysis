#pragma once

#include <string>
#include "value.h"

Json::Value ToJson(const std::string json);

Json::Value ToJson(const char *json);

Json::Value ToJson(const char *json, int len);

Json::Value ReadFileJson(const std::string &path);

std::string Json2String(const Json::Value &json) ;