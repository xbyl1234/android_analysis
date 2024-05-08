#pragma once

#include <vector>
#include <string>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include "utils.h"

int pid_by_process_name(char *task_name);

std::string Fd2Path(int fd);

bool Mount(const std::string &fakePath, const std::string &targetPath);

bool UnMount(const std::string &targetPath);

std::string GetCmdLine(pid_t pid);

std::string RunCmd(const std::string &strCmd);

struct MountInfo {
    std::string mnt_fsname;
    std::string mnt_dir;
    std::string mnt_type;
    std::string mnt_opts;
    int mnt_freq;
    int mnt_passno;
};

bool get_mount_list(const char *path, std::vector<MountInfo> &ret);

bool copy_file(const std::string &from, const std::string &to);

struct FilePerm {
    std::string ctx;
    mode_t st_mode;
    gid_t st_gid;
    uid_t st_uid;
};

bool
SetFilePerm(const std::string &path, const std::string &context, mode_t st_mode, gid_t st_gid,
            uid_t st_uid);

bool GetFilePerm(const std::string &path, FilePerm &perm);

bool GetFileSelinuxCtx(const std::string &path, std::string &ctx);

bool remove_dir(const std::string &dir_name);

unsigned long get_file_size(const std::string &path);

bool remove_files(const std::string &path);

bool remove_dir(const string &rootPath, const vector<string> &white, bool deleteRoot);

bool UnMount2(const char *target, int flags);

bool UnMount(const char *target);

bool Mount(const char *source, const char *target, const char *fs_type, unsigned long flags,
           const void *data);

bool mkdir_recursive(const std::string &path, const mode_t mode);

enum class FileType {
    file,
    dir,
    link2file,
    link2dir,
    unknown,
};

FileType get_path_type(const string &path, struct stat *statFile, struct stat *statLink);

bool
traverse_path(const std::string &rootPath,
              const std::function<bool(const string &path, const string &name,
                                       int mode)> &callback,
              const vector<string> &whitePath = vector<string>());

template<class Func, class... Args>
bool singleCase(const string &filePath, bool *isSingle, Func &&callback, Args &&... args) {
    int lockfile = open(filePath.c_str(), O_CREAT | O_RDWR, 0666);
    if (lockfile == -1) {
        if (isSingle) {
            *isSingle = false;
        }
        return false;
    }
    defer([&]() {
        close(lockfile);
    });
    flock fl{};
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    if (fcntl(lockfile, F_SETLK, &fl) == -1) {
        if (isSingle) {
            *isSingle = false;
        }
        return false;
    }
    defer([&]() {
        fl.l_type = F_UNLCK;
        fcntl(lockfile, F_SETLK, &fl);
    });
    if (isSingle) {
        *isSingle = true;
    }
    callback(std::forward<Args>(args)...);
    return true;
}

struct MapsInfo {
    void *region_start;
    void *region_end;
    void *region_offset;
    std::string permissions;
    std::string path;
};

class MapsHelper {
public:
    vector<MapsInfo> mapsInfo;

    int refresh(const string &libPath = "", const string &wantPerm = "");

    int refresh_reg(const string &libPath = "", const string &wantPerm = "");

    void *get_module_base(const string &libPath);
    void *get_module_base_reg(const string &libPath);

    void *get_module_end(const string &libPath);
    void *get_module_end_reg(const string &libPath);
private:
    void *get_module_base(const string &libPath, bool is_reg);
    void *get_module_end(const string &libPath, bool is_reg);

    bool get_process_maps(bool is_reg, const string &libPath = "", const string &wantPerm = "");
};
