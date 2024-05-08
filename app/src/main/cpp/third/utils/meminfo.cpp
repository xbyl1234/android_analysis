#include <string>
#include <map>
#include "meminfo.h"

using namespace std;

enum MemInfoKeyIndex {
    MemTotal = 0,
    MemFree,
    MemAvailable,
    Buffers,
    Cached,
    SwapCached,
    Active,
    Inactive,
    Active_anon,
    Inactive_anon,
    Active_file,
    Inactive_file,
    Unevictable,
    Mlocked,
    SwapTotal,
    SwapFree,
    Dirty,
    Writeback,
    AnonPages,
    Mapped,
    Shmem,
    Slab,
    SReclaimable,
    SUnreclaim,
    KernelStack,
    PageTables,
    NFS_Unstable,
    Bounce,
    WritebackTmp,
    CommitLimit,
    Committed_AS,
    VmallocTotal,
    VmallocUsed,
    VmallocChunk,
    CmaTotal,
    CmaFree,
};

map<string, MemInfoKeyIndex> MemInfoOffset = {
        {"MemTotal:",       MemTotal},
        {"MemFree:",        MemFree},
        {"MemAvailable:",   MemAvailable},
        {"Buffers:",        Buffers},
        {"Cached:",         Cached},
        {"SwapCached:",     SwapCached},
        {"Active:",         Active},
        {"Inactive:",       Inactive},
        {"Active(anon):",   Active_anon},
        {"Inactive(anon):", Inactive_anon},
        {"Active(file):",   Active_file},
        {"Inactive(file):", Inactive_file},
        {"Unevictable:",    Unevictable},
        {"Mlocked:",        Mlocked},
        {"SwapTotal:",      SwapTotal},
        {"SwapFree:",       SwapFree},
        {"Dirty:",          Dirty},
        {"Writeback:",      Writeback},
        {"AnonPages:",      AnonPages},
        {"Mapped:",         Mapped},
        {"Shmem:",          Shmem},
        {"Slab:",           Slab},
        {"SReclaimable:",   SReclaimable},
        {"SUnreclaim:",     SUnreclaim},
        {"KernelStack:",    KernelStack},
        {"PageTables:",     PageTables},
        {"NFS_Unstable:",   NFS_Unstable},
        {"Bounce:",         Bounce},
        {"WritebackTmp:",   WritebackTmp},
        {"CommitLimit:",    CommitLimit},
        {"Committed_AS:",   Committed_AS},
        {"VmallocTotal:",   VmallocTotal},
        {"VmallocUsed:",    VmallocUsed},
        {"VmallocChunk:",   VmallocChunk},
        {"CmaTotal:",       CmaTotal},
        {"CmaFree:",        CmaFree},
};

bool GetMemInfo(const string &path, MemInfo *memInfo) {
    return GetMemInfo(path, memInfo, -1);
}

bool GetMemInfo(const string &path, MemInfo *memInfo, int maxLine) {
    memset(memInfo, 0, sizeof(MemInfo));
    FILE *fp;
    char buf[128];
    fp = fopen(path.c_str(), "r");
    if (fp == NULL) {
        return false;
    }
    if (maxLine == -1) {
        maxLine = 99999;
    }
    char name[20];
    int value;
    while (NULL != fgets(buf, sizeof(buf), fp) && maxLine > 0) {
        maxLine--;
        sscanf(buf, "%s%d", name, &value);
        const auto &item = MemInfoOffset.find(name);
        if (item == MemInfoOffset.end()) {
            continue;
        }
        *((int *) memInfo + (int) item->second) = value;
    }
    fclose(fp);
    return true;
}