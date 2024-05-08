#include "../third//utils/log.h"
#include "../third//tinyxml2/tinyxml2.h"

using namespace tinyxml2;

bool system_status(int status) {
    if (status == -1)
        return false;
    if (!WIFEXITED(status))
        return false;
    if (0 != WEXITSTATUS(status))
        return false;
    return true;
}

void system_call(const string &cmd) {
    int ret = system(cmd.c_str());
    if (!system_status(ret)) {
        throw cmd;
    }
}

int main(int argc, char **args) {
    logi("%s", "start sniffer...");
    if (argc != 3) {
        loge("%s", "args error!");
        return 0;
    }
    XMLDocument doc;
    doc.LoadFile("/data/system/packages.xml");
    if (doc.Error()) {
        loge("read packages.xml error: %s", doc.ErrorStr());
        return 0;
    }
    string package_name = args[1];
    string file_name = args[2];
    string userid;
    auto packages = doc.FirstChildElement("packages");
    XMLElement *package = packages->FirstChildElement("package");
    while (package) {
        if (package_name == package->Attribute("name")) {
            userid = package->Attribute("userId");
        }
        package = package->NextSiblingElement("package");
    }
    if (userid.empty()) {
        loge("%s", "cant finde userid!");
        return 0;
    }
    logi("get package userid: %s", userid.c_str());
    try {
        system_call(format_string(
                "iptables -A OUTPUT -m owner --uid-owner %s -j CONNMARK --set-mark %s",
                userid.c_str(), userid.c_str()));
        system_call(
                format_string("iptables -A INPUT -m connmark --mark %s -j NFLOG --nflog-group %s",
                              userid.c_str(), userid.c_str()));
        system_call(
                format_string("iptables -A OUTPUT -m connmark --mark %s -j NFLOG --nflog-group %s",
                              userid.c_str(), userid.c_str()));

        system_call(format_string("./tcpdump -i nflog:%s -w /sdcard/%s_%s.pcap", userid.c_str(),
                                  package_name.c_str(), file_name.c_str()));
    } catch (const string &err_cmd) {
        loge("cmd run error: %s", err_cmd.c_str());
    }

    return 0;
}