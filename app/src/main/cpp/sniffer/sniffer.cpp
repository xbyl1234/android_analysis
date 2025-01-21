#include "../third/utils/log.h"
#include "../third/utils/linux_helper.h"
#include "../third/tinyxml2/tinyxml2.h"

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
    init_log("sniffer", new xbyl::adapter_printf());
    logi("%s", "start sniffer...");
    if (argc != 3) {
        loge("%s", "args error!");
        return 0;
    }
    string package_name = args[1];
    string file_name = args[2];
    string userid;
    string result = RunCmd("cmd package list packages -U |grep " + package_name);
    if (result.find("uid:") == -1) {
        loge("not find pkg!");
        return 0;
    }
    userid = replace_all(result.substr(result.find("uid:") + 4), "\n", "");
    userid = replace_all(userid, "\r", "");
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