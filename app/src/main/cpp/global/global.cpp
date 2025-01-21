#include "global.h"

using namespace std;
static string pkgName;


string getPkgName() {
    return pkgName;
}

void setPkgName(const string &name) {
    pkgName = name;
}
