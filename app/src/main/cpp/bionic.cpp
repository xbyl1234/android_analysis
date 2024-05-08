//
// Created by xbyl on 2023/12/6.
//

#include "bionic.h"
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cwchar>

namespace xbyl {
    char *
    strcpy(char *to, const char *from) {
        char *save = to;

        for (; (*to = *from) != '\0'; ++from, ++to);
        return (save);
    }

    size_t strlen(const char *str) {
        const char *s;
        for (s = str; *s; ++s);
        return (s - str);
    }
}