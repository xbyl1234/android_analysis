#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <dlfcn.h>
#include <execinfo.h>
#include <netdb.h>
#include <map>

using namespace std;

#include "third/dobby/include/dobby.h"
#include "third/utils/utils.h"
#include "third/utils/log.h"
#include "third/utils/jni_helper.hpp"
#include "third/utils/linux_helper.h"

using namespace std;

typedef int(*pfnAndroidGetAddrInfoForNet)(const char *hostname, const char *servname,
                                          const struct addrinfo *hints, unsigned netid,
                                          unsigned mark, struct addrinfo **res);

typedef int(*pfnGetAddrInfo)(const char *node, const char *service,
                             const struct addrinfo *hints, struct addrinfo **result);

typedef int (*pfnFakeConnect)(int fd, const struct sockaddr *addr, socklen_t addr_length);


map<string, string> fakeHostMap;
recursive_mutex hostLock;
pfnAndroidGetAddrInfoForNet orig_android_getaddrinfofornet;
pfnGetAddrInfo orig_getaddrinfo;
pfnFakeConnect orig_connect;

string GetFakeHostIp(const string &host) {
    for (auto &item: fakeHostMap) {
        if (host.ends_with(item.first)) {
            LOGI("fake %s to %s", item.first.c_str(), item.second.c_str());
            return item.second;
        }
    }
    LOGI("not fake %s", host.c_str());
    return "";
}

int fakeAndroidGetAddrInfoForNet(const char *hostname, const char *servname,
                                 const struct addrinfo *hints, unsigned netid,
                                 unsigned mark, struct addrinfo **res) {
//    logi("fakeAndroidGetAddrInfoForNet %s", hostname);
    LOGI("fakeAndroidGetAddrInfoForNet %s", hostname);
    lock_guard<recursive_mutex> guard(hostLock);
    if (hints->ai_flags != AI_NUMERICHOST) {
        auto ip = GetFakeHostIp(hostname);
        if (!ip.empty()) {
            return orig_android_getaddrinfofornet(ip.c_str(), servname, hints, netid, mark, res);
        }
    }
    return orig_android_getaddrinfofornet(hostname, servname, hints, netid, mark, res);
}

int fakeGetaddrinfo(const char *node, const char *service, const struct addrinfo *hints,
                    struct addrinfo **result) {
//    logi("fakeGetaddrinfo %s", node);
    LOGI("fakeGetaddrinfo %s", node);
    lock_guard<recursive_mutex> guard(hostLock);
    auto ip = GetFakeHostIp(node);
    if (!ip.empty()) {
        return orig_getaddrinfo(ip.c_str(), service, hints, result);
    }
    return orig_getaddrinfo(node, service, hints, result);
}

string SockAddr2String(const struct sockaddr *addr) {
    const sockaddr_in *addrIn = (sockaddr_in *) addr;
    char ip_address[INET_ADDRSTRLEN]{};
    inet_ntop(addrIn->sin_family, &(addrIn->sin_addr), ip_address, INET_ADDRSTRLEN);
    return format_string("%s:%d", ip_address, ntohs(addrIn->sin_port));
}

static sockaddr_in LocalServerAddr;

int fakeConnect(int fd, const struct sockaddr *addr, socklen_t addr_length) {
    if (addr == nullptr || addr->sa_family != AF_INET) {
        ret:
        return orig_connect(fd, addr, addr_length);
    }

    string sockAddrStr = SockAddr2String(addr);
    LOGI("connect to %s", sockAddrStr.c_str());
    if (sockAddrStr.find("127.0.0.1:443") != -1) {
        LOGI("fake %s connect to 10443", sockAddrStr.c_str());
        return orig_connect(fd, (sockaddr *) (&LocalServerAddr), sizeof(sockaddr_in));
    }
    goto ret;
}


static bool hadHook = false;
using func_hook_dlopen = void *(*)(const char *filename, int flag);
static func_hook_dlopen phook_dlopen;

__attribute__((__weak__))
void *hook_dlopen(const char *filename, int flag) {
    void *ret = phook_dlopen(filename, flag);
    if (!hadHook && strstr(filename, "libuc_bhook_jni.so")) {
        logi("dlopen load target library finish!");
        hadHook = true;
    } else {
        logi("dlopen load library %s", filename);
    }
    return ret;
}


void hook_dns() {
//    xbyl::init_log("xp_helper", new xbyl::adapter_adb());
    LOGI("hook dns inject!");
//    fakeHostMap[".gvt1.com"] = "192.168.31.163";
//    fakeHostMap[".gvt1.com"] = "192.168.31.162";
    fakeHostMap[".gvt1.com"] = "127.0.0.1";
    memset(&LocalServerAddr, 0, sizeof(LocalServerAddr));
    LocalServerAddr.sin_family = AF_INET;
    LocalServerAddr.sin_port = htons(10443);
    *(uint32_t *) &LocalServerAddr.sin_addr = htonl(0x7f000001U);


    DobbyHook(DobbySymbolResolver("libc.so", "getaddrinfo"),
              (dobby_dummy_func_t) &fakeGetaddrinfo,
              (dobby_dummy_func_t *) &orig_getaddrinfo);
    DobbyHook(DobbySymbolResolver("libc.so", "android_getaddrinfofornet"),
              (dobby_dummy_func_t) &fakeAndroidGetAddrInfoForNet,
              (dobby_dummy_func_t *) &orig_android_getaddrinfofornet);
    DobbyHook(DobbySymbolResolver("libc.so", "connect"),
              (dobby_dummy_func_t) &fakeConnect,
              (dobby_dummy_func_t *) &orig_connect);


    DobbyHook(DobbySymbolResolver("libdl.so", "dlopen"), (dobby_dummy_func_t) hook_dlopen,
              (dobby_dummy_func_t *) &phook_dlopen);
}
