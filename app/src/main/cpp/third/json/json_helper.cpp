#include <cstdio>
#include <fstream>
#include "json.h"

using std::string;
using std::ifstream;

Json::Value ReadFileJson(const string &path) {
    Json::Value json;
    ifstream file(path);
    if (!file) {
        return json;
    }
    Json::Reader reader;
    if (!reader.parse(file, json, false)) {
        return json;
    }
    return json;
}

Json::Value ToJson(const std::string json) {
    Json::Value ret;
    Json::Reader reader;
    if (!reader.parse(json, ret, false)) {
        return Json::nullValue;
    }
    return ret;
}

Json::Value ToJson(const char *json) {
    Json::Value ret;
    Json::Reader reader;
    if (!reader.parse(json, ret, false)) {
        return Json::nullValue;
    }
    return ret;
}

Json::Value ToJson(const char *json, int len) {
    Json::Value ret;
    Json::Reader reader;
    if (!reader.parse(json, json + len, ret, false)) {
        return Json::nullValue;
    }
    return ret;
}

string Json2String(const Json::Value &json) {
    Json::FastWriter writer;
    return writer.write(json);
}
