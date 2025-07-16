#include "global.h"

using namespace std;
static string pkgName;

string get_packet_name();

string getPkgName() {
    if (pkgName.empty()) {
        pkgName = get_packet_name();
    }
    return pkgName;
}

void setPkgName(const string &name) {
    pkgName = name;
}
