#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cassert>
#include <algorithm>


class ArgumentParser {
private:

    std::map<std::string, std::string> params;

public:
    ArgumentParser() {

    }

    bool parse(int argc, char *argv[]) {
        int index = 0;
        auto getNext = [&]() -> char * {
            if (index >= argc) {
                return nullptr;
            }
            return argv[index++];
        };

        auto pickNext = [&]() -> char * {
            if (index >= argc) {
                return nullptr;
            }
            return argv[index];
        };
        getNext();

        char *cur = nullptr;
        while (cur = getNext(), cur != nullptr) {
            std::string arg = cur;
            if (arg.empty() || arg.length() <= 1) {
                return false;
            }
            if (arg.substr(0, 1) == "-") {
                std::string key = arg.substr(1);
                char *next = pickNext();
                if (next == nullptr || next[0] == '-') {
                    params[key] = "";
                } else {
                    params[key] = getNext();
                }
            } else {
                return false;
            }
        }
        return true;
    }

    bool exists(const std::string &key) {
        return params.find(key) != params.end();
    }

    template<class T>
    T get(const std::string &key) {

    }

    template<>
    string get(const std::string &key) {
        return params[key];
    }

    template<>
    int get(const std::string &key) {
        return stoi(params[key]);
    }

    template<>
    bool get(const std::string &key) {
        string value = params[key];
        return value == "true";
    }

    void addArgument(string argument) {

    }
};
