#pragma once

#include <string.h>

struct MemInfo {
    int MemTotal;
    int MemFree;
    int MemAvailable;
    int Buffers;
    int Cached;
    int SwapCached;
    int Active;
    int Inactive;
    int Active_anon;
    int Inactive_anon;
    int Active_file;
    int Inactive_file;
    int Unevictable;
    int Mlocked;
    int SwapTotal;
    int SwapFree;
    int Dirty;
    int Writeback;
    int AnonPages;
    int Mapped;
    int Shmem;
    int Slab;
    int SReclaimable;
    int SUnreclaim;
    int KernelStack;
    int PageTables;
    int NFS_Unstable;
    int Bounce;
    int WritebackTmp;
    int CommitLimit;
    int Committed_AS;
    int VmallocTotal;
    int VmallocUsed;
    int VmallocChunk;
    int CmaTotal;
    int CmaFree;
};

bool GetMemInfo(const std::string &path, MemInfo *memInfo);

bool GetMemInfo(const std::string &path, MemInfo *memInfo, int maxLine);


