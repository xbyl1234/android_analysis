
//2AAF8
#include <stdio.h>
#include <thread>
#include <map>
#include <sys/sysinfo.h>


#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/log.h"
#include "../third/utils/utils.h"
#include "../common_hook.h"
#include "../dump_so.h"
#include "../base/hook.h"
#include "../base/when_hook.h"
#include "../third/log2file/app_file_writer.h"


using namespace std;

#define TargetLibName   "libnms.so"
extern char *targetLibBase;

DefineHookStub(decode_str, char*, char *k1, char *str, char *k2) {
    auto result = pHook_decode_str(k1, str, k2);
    uint64_t retAddr = (uint64_t) __builtin_return_address(0) - (uint64_t) targetLibBase;
    log2file("encode_str: %p %s", retAddr, str);
    return result;
}

DefineHookStub(hook_jump, void*, void *k1, void *k2) {
    uint64_t retAddr = (uint64_t) __builtin_return_address(0) - (uint64_t) targetLibBase;
    log2file("hook_jump: %p, ret: %p", (uint64_t) k1 - (uint64_t) targetLibBase, retAddr);
    return pHook_hook_jump(k1, k2);
}

DefineHookStub(decode_str2, char*, char *k1, int len, char *k3, int len2, char *str) {
    auto result = pHook_decode_str2(k1, len, k3, len2, str);
    uint64_t retAddr = (uint64_t) __builtin_return_address(0) - (uint64_t) targetLibBase;
    log2file("decode_str2: %s, %p", string(str, len).c_str(), retAddr);
    return result;
}


#include <asm/unistd.h>

map<uint64_t, string> syscall2str{
        {__NR_io_setup,                "io_setup"},
        {__NR_io_destroy,              "io_destroy"},
        {__NR_io_submit,               "io_submit"},
        {__NR_io_cancel,               "io_cancel"},
        {__NR_io_getevents,            "io_getevents"},
        {__NR_setxattr,                "setxattr"},
        {__NR_lsetxattr,               "lsetxattr"},
        {__NR_fsetxattr,               "fsetxattr"},
        {__NR_getxattr,                "getxattr"},
        {__NR_lgetxattr,               "lgetxattr"},
        {__NR_fgetxattr,               "fgetxattr"},
        {__NR_listxattr,               "listxattr"},
        {__NR_llistxattr,              "llistxattr"},
        {__NR_flistxattr,              "flistxattr"},
        {__NR_removexattr,             "removexattr"},
        {__NR_lremovexattr,            "lremovexattr"},
        {__NR_fremovexattr,            "fremovexattr"},
        {__NR_getcwd,                  "getcwd"},
        {__NR_lookup_dcookie,          "lookup_dcookie"},
        {__NR_eventfd2,                "eventfd2"},
        {__NR_epoll_create1,           "epoll_create1"},
        {__NR_epoll_ctl,               "epoll_ctl"},
        {__NR_epoll_pwait,             "epoll_pwait"},
        {__NR_dup,                     "dup"},
        {__NR_dup3,                    "dup3"},
//        {               __NR3264_fcntl,"264_fcntl"},
        {__NR_inotify_init1,           "inotify_init1"},
        {__NR_inotify_add_watch,       "inotify_add_watch"},
        {__NR_inotify_rm_watch,        "inotify_rm_watch"},
        {__NR_ioctl,                   "ioctl"},
        {__NR_ioprio_set,              "ioprio_set"},
        {__NR_ioprio_get,              "ioprio_get"},
        {__NR_flock,                   "flock"},
        {__NR_mknodat,                 "mknodat"},
        {__NR_mkdirat,                 "mkdirat"},
        {__NR_unlinkat,                "unlinkat"},
        {__NR_symlinkat,               "symlinkat"},
        {__NR_linkat,                  "linkat"},
        {__NR_renameat,                "renameat"},
        {__NR_umount2,                 "umount2"},
        {__NR_mount,                   "mount"},
        {__NR_pivot_root,              "pivot_root"},
        {__NR_nfsservctl,              "nfsservctl"},
//        {              __NR3264_statfs,"264_statfs"},
//        {             __NR3264_fstatfs,"264_fstatfs"},
//        {            __NR3264_truncate,"264_truncate"},
//        {           __NR3264_ftruncate,"264_ftruncate"},
        {__NR_fallocate,               "fallocate"},
        {__NR_faccessat,               "faccessat"},
        {__NR_chdir,                   "chdir"},
        {__NR_fchdir,                  "fchdir"},
        {__NR_chroot,                  "chroot"},
        {__NR_fchmod,                  "fchmod"},
        {__NR_fchmodat,                "fchmodat"},
        {__NR_fchownat,                "fchownat"},
        {__NR_fchown,                  "fchown"},
        {__NR_openat,                  "openat"},
        {__NR_close,                   "close"},
        {__NR_vhangup,                 "vhangup"},
        {__NR_pipe2,                   "pipe2"},
        {__NR_quotactl,                "quotactl"},
        {__NR_getdents64,              "getdents64"},
//        {               __NR3264_lseek,"264_lseek"},
        {__NR_read,                    "read"},
        {__NR_write,                   "write"},
        {__NR_readv,                   "readv"},
        {__NR_writev,                  "writev"},
        {__NR_pread64,                 "pread64"},
        {__NR_pwrite64,                "pwrite64"},
        {__NR_preadv,                  "preadv"},
        {__NR_pwritev,                 "pwritev"},
//        {            __NR3264_sendfile,"264_sendfile"},
        {__NR_pselect6,                "pselect6"},
        {__NR_ppoll,                   "ppoll"},
        {__NR_signalfd4,               "signalfd4"},
        {__NR_vmsplice,                "vmsplice"},
        {__NR_splice,                  "splice"},
        {__NR_tee,                     "tee"},
        {__NR_readlinkat,              "readlinkat"},
//        {             __NR3264_fstatat,"264_fstatat"},
//        {               __NR3264_fstat,"264_fstat"},
        {__NR_sync,                    "sync"},
        {__NR_fsync,                   "fsync"},
        {__NR_fdatasync,               "fdatasync"},
//        {             __NR_sync_file_range2,"sync_file_range2"},
        {__NR_sync_file_range,         "sync_file_range"},
        {__NR_timerfd_create,          "timerfd_create"},
        {__NR_timerfd_settime,         "timerfd_settime"},
        {__NR_timerfd_gettime,         "timerfd_gettime"},
        {__NR_utimensat,               "utimensat"},
        {__NR_acct,                    "acct"},
        {__NR_capget,                  "capget"},
        {__NR_capset,                  "capset"},
        {__NR_personality,             "personality"},
        {__NR_exit,                    "exit"},
        {__NR_exit_group,              "exit_group"},
        {__NR_waitid,                  "waitid"},
        {__NR_set_tid_address,         "set_tid_address"},
        {__NR_unshare,                 "unshare"},
        {__NR_futex,                   "futex"},
        {__NR_set_robust_list,         "set_robust_list"},
        {__NR_get_robust_list,         "get_robust_list"},
        {__NR_nanosleep,               "nanosleep"},
        {__NR_getitimer,               "getitimer"},
        {__NR_setitimer,               "setitimer"},
        {__NR_kexec_load,              "kexec_load"},
        {__NR_init_module,             "init_module"},
        {__NR_delete_module,           "delete_module"},
        {__NR_timer_create,            "timer_create"},
        {__NR_timer_gettime,           "timer_gettime"},
        {__NR_timer_getoverrun,        "timer_getoverrun"},
        {__NR_timer_settime,           "timer_settime"},
        {__NR_timer_delete,            "timer_delete"},
        {__NR_clock_settime,           "clock_settime"},
        {__NR_clock_gettime,           "clock_gettime"},
        {__NR_clock_getres,            "clock_getres"},
        {__NR_clock_nanosleep,         "clock_nanosleep"},
        {__NR_syslog,                  "syslog"},
        {__NR_ptrace,                  "ptrace"},
        {__NR_sched_setparam,          "sched_setparam"},
        {__NR_sched_setscheduler,      "sched_setscheduler"},
        {__NR_sched_getscheduler,      "sched_getscheduler"},
        {__NR_sched_getparam,          "sched_getparam"},
        {__NR_sched_setaffinity,       "sched_setaffinity"},
        {__NR_sched_getaffinity,       "sched_getaffinity"},
        {__NR_sched_yield,             "sched_yield"},
        {__NR_sched_get_priority_max,  "sched_get_priority_max"},
        {__NR_sched_get_priority_min,  "sched_get_priority_min"},
        {__NR_sched_rr_get_interval,   "sched_rr_get_interval"},
        {__NR_restart_syscall,         "restart_syscall"},
        {__NR_kill,                    "kill"},
        {__NR_tkill,                   "tkill"},
        {__NR_tgkill,                  "tgkill"},
        {__NR_sigaltstack,             "sigaltstack"},
        {__NR_rt_sigsuspend,           "rt_sigsuspend"},
        {__NR_rt_sigaction,            "rt_sigaction"},
        {__NR_rt_sigprocmask,          "rt_sigprocmask"},
        {__NR_rt_sigpending,           "rt_sigpending"},
        {__NR_rt_sigtimedwait,         "rt_sigtimedwait"},
        {__NR_rt_sigqueueinfo,         "rt_sigqueueinfo"},
        {__NR_rt_sigreturn,            "rt_sigreturn"},
        {__NR_setpriority,             "setpriority"},
        {__NR_getpriority,             "getpriority"},
        {__NR_reboot,                  "reboot"},
        {__NR_setregid,                "setregid"},
        {__NR_setgid,                  "setgid"},
        {__NR_setreuid,                "setreuid"},
        {__NR_setuid,                  "setuid"},
        {__NR_setresuid,               "setresuid"},
        {__NR_getresuid,               "getresuid"},
        {__NR_setresgid,               "setresgid"},
        {__NR_getresgid,               "getresgid"},
        {__NR_setfsuid,                "setfsuid"},
        {__NR_setfsgid,                "setfsgid"},
        {__NR_times,                   "times"},
        {__NR_setpgid,                 "setpgid"},
        {__NR_getpgid,                 "getpgid"},
        {__NR_getsid,                  "getsid"},
        {__NR_setsid,                  "setsid"},
        {__NR_getgroups,               "getgroups"},
        {__NR_setgroups,               "setgroups"},
        {__NR_uname,                   "uname"},
        {__NR_sethostname,             "sethostname"},
        {__NR_setdomainname,           "setdomainname"},
        {__NR_getrlimit,               "getrlimit"},
        {__NR_setrlimit,               "setrlimit"},
        {__NR_getrusage,               "getrusage"},
        {__NR_umask,                   "umask"},
        {__NR_prctl,                   "prctl"},
        {__NR_getcpu,                  "getcpu"},
        {__NR_gettimeofday,            "gettimeofday"},
        {__NR_settimeofday,            "settimeofday"},
        {__NR_adjtimex,                "adjtimex"},
        {__NR_getpid,                  "getpid"},
        {__NR_getppid,                 "getppid"},
        {__NR_getuid,                  "getuid"},
        {__NR_geteuid,                 "geteuid"},
        {__NR_getgid,                  "getgid"},
        {__NR_getegid,                 "getegid"},
        {__NR_gettid,                  "gettid"},
        {__NR_sysinfo,                 "sysinfo"},
        {__NR_mq_open,                 "mq_open"},
        {__NR_mq_unlink,               "mq_unlink"},
        {__NR_mq_timedsend,            "mq_timedsend"},
        {__NR_mq_timedreceive,         "mq_timedreceive"},
        {__NR_mq_notify,               "mq_notify"},
        {__NR_mq_getsetattr,           "mq_getsetattr"},
        {__NR_msgget,                  "msgget"},
        {__NR_msgctl,                  "msgctl"},
        {__NR_msgrcv,                  "msgrcv"},
        {__NR_msgsnd,                  "msgsnd"},
        {__NR_semget,                  "semget"},
        {__NR_semctl,                  "semctl"},
        {__NR_semtimedop,              "semtimedop"},
        {__NR_semop,                   "semop"},
        {__NR_shmget,                  "shmget"},
        {__NR_shmctl,                  "shmctl"},
        {__NR_shmat,                   "shmat"},
        {__NR_shmdt,                   "shmdt"},
        {__NR_socket,                  "socket"},
        {__NR_socketpair,              "socketpair"},
        {__NR_bind,                    "bind"},
        {__NR_listen,                  "listen"},
        {__NR_accept,                  "accept"},
        {__NR_connect,                 "connect"},
        {__NR_getsockname,             "getsockname"},
        {__NR_getpeername,             "getpeername"},
        {__NR_sendto,                  "sendto"},
        {__NR_recvfrom,                "recvfrom"},
        {__NR_setsockopt,              "setsockopt"},
        {__NR_getsockopt,              "getsockopt"},
        {__NR_shutdown,                "shutdown"},
        {__NR_sendmsg,                 "sendmsg"},
        {__NR_recvmsg,                 "recvmsg"},
        {__NR_readahead,               "readahead"},
        {__NR_brk,                     "brk"},
        {__NR_munmap,                  "munmap"},
        {__NR_mremap,                  "mremap"},
        {__NR_add_key,                 "add_key"},
        {__NR_request_key,             "request_key"},
        {__NR_keyctl,                  "keyctl"},
        {__NR_clone,                   "clone"},
        {__NR_execve,                  "execve"},
//        {                __NR3264_mmap,"264_mmap"},
//        {           __NR3264_fadvise64,"264_fadvise64"},
        {__NR_swapon,                  "swapon"},
        {__NR_swapoff,                 "swapoff"},
        {__NR_mprotect,                "mprotect"},
        {__NR_msync,                   "msync"},
        {__NR_mlock,                   "mlock"},
        {__NR_munlock,                 "munlock"},
        {__NR_mlockall,                "mlockall"},
        {__NR_munlockall,              "munlockall"},
        {__NR_mincore,                 "mincore"},
        {__NR_madvise,                 "madvise"},
        {__NR_remap_file_pages,        "remap_file_pages"},
        {__NR_mbind,                   "mbind"},
        {__NR_get_mempolicy,           "get_mempolicy"},
        {__NR_set_mempolicy,           "set_mempolicy"},
        {__NR_migrate_pages,           "migrate_pages"},
        {__NR_move_pages,              "move_pages"},
        {__NR_rt_tgsigqueueinfo,       "rt_tgsigqueueinfo"},
        {__NR_perf_event_open,         "perf_event_open"},
        {__NR_accept4,                 "accept4"},
        {__NR_recvmmsg,                "recvmmsg"},
//        {   __NR_arch_specific_syscall,"arch_specific_syscall"},
        {__NR_wait4,                   "wait4"},
        {__NR_prlimit64,               "prlimit64"},
        {__NR_fanotify_init,           "fanotify_init"},
        {__NR_fanotify_mark,           "fanotify_mark"},
        {__NR_name_to_handle_at,       "name_to_handle_at"},
        {__NR_open_by_handle_at,       "open_by_handle_at"},
        {__NR_clock_adjtime,           "clock_adjtime"},
        {__NR_syncfs,                  "syncfs"},
        {__NR_setns,                   "setns"},
        {__NR_sendmmsg,                "sendmmsg"},
        {__NR_process_vm_readv,        "process_vm_readv"},
        {__NR_process_vm_writev,       "process_vm_writev"},
        {__NR_kcmp,                    "kcmp"},
        {__NR_finit_module,            "finit_module"},
        {__NR_sched_setattr,           "sched_setattr"},
        {__NR_sched_getattr,           "sched_getattr"},
        {__NR_renameat2,               "renameat2"},
        {__NR_seccomp,                 "seccomp"},
        {__NR_getrandom,               "getrandom"},
        {__NR_memfd_create,            "memfd_create"},
        {__NR_bpf,                     "bpf"},
        {__NR_execveat,                "execveat"},
        {__NR_userfaultfd,             "userfaultfd"},
        {__NR_membarrier,              "membarrier"},
        {__NR_mlock2,                  "mlock2"},
        {__NR_copy_file_range,         "copy_file_range"},
        {__NR_preadv2,                 "preadv2"},
        {__NR_pwritev2,                "pwritev2"},
        {__NR_pkey_mprotect,           "pkey_mprotect"},
        {__NR_pkey_alloc,              "pkey_alloc"},
        {__NR_pkey_free,               "pkey_free"},
        {__NR_statx,                   "statx"},
        {__NR_io_pgetevents,           "io_pgetevents"},
        {__NR_rseq,                    "rseq"},
        {__NR_kexec_file_load,         "kexec_file_load"},
//        {              __NR_clock_gettime64,"clock_gettime64"},
//        {              __NR_clock_settime64,"clock_settime64"},
//        {              __NR_clock_adjtime64,"clock_adjtime64"},
//        {          __NR_clock_getres_time64,"clock_getres_time64"},
//        {       __NR_clock_nanosleep_time64,"clock_nanosleep_time64"},
//        {              __NR_timer_gettime64,"timer_gettime64"},
//        {              __NR_timer_settime64,"timer_settime64"},
//        {            __NR_timerfd_gettime64,"timerfd_gettime64"},
//        {            __NR_timerfd_settime64,"timerfd_settime64"},
//        {             __NR_utimensat_time64,"utimensat_time64"},
//        {              __NR_pselect6_time64,"pselect6_time64"},
//        {                 __NR_ppoll_time64,"ppoll_time64"},
//        {         __NR_io_pgetevents_time64,"io_pgetevents_time64"},
//        {              __NR_recvmmsg_time64,"recvmmsg_time64"},
//        {          __NR_mq_timedsend_time64,"mq_timedsend_time64"},
//        {       __NR_mq_timedreceive_time64,"mq_timedreceive_time64"},
//        {            __NR_semtimedop_time64,"semtimedop_time64"},
//        {       __NR_rt_sigtimedwait_time64,"rt_sigtimedwait_time64"},
//        {                 __NR_futex_time64,"futex_time64"},
//        { __NR_sched_rr_get_interval_time64,"sched_rr_get_interval_time64"},
        {__NR_pidfd_send_signal,       "pidfd_send_signal"},
        {__NR_io_uring_setup,          "io_uring_setup"},
        {__NR_io_uring_enter,          "io_uring_enter"},
        {__NR_io_uring_register,       "io_uring_register"},
        {__NR_open_tree,               "open_tree"},
        {__NR_move_mount,              "move_mount"},
        {__NR_fsopen,                  "fsopen"},
        {__NR_fsconfig,                "fsconfig"},
        {__NR_fsmount,                 "fsmount"},
        {__NR_fspick,                  "fspick"},
        {__NR_pidfd_open,              "pidfd_open"},
        {__NR_clone3,                  "clone3"},
        {__NR_close_range,             "close_range"},
        {__NR_openat2,                 "openat2"},
        {__NR_pidfd_getfd,             "pidfd_getfd"},
        {__NR_faccessat2,              "faccessat2"},
        {__NR_process_madvise,         "process_madvise"},
        {__NR_epoll_pwait2,            "epoll_pwait2"},
        {__NR_mount_setattr,           "mount_setattr"},
        {__NR_quotactl_fd,             "quotactl_fd"},
        {__NR_landlock_create_ruleset, "landlock_create_ruleset"},
        {__NR_landlock_add_rule,       "landlock_add_rule"},
        {__NR_landlock_restrict_self,  "landlock_restrict_self"},
        {__NR_memfd_secret,            "memfd_secret"},
//        {                __NR_syscalls,"syscalls"},
        {__NR_fcntl,                   "fcntl"},
        {__NR_statfs,                  "statfs"},
        {__NR_fstatfs,                 "fstatfs"},
        {__NR_truncate,                "truncate"},
        {__NR_ftruncate,               "ftruncate"},
        {__NR_lseek,                   "lseek"},
        {__NR_sendfile,                "sendfile"},
        {__NR_newfstatat,              "newfstatat"},
        {__NR_fstat,                   "fstat"},
        {__NR_mmap,                    "mmap"},
        {__NR_fadvise64,               "fadvise64"},
//        {                         __NR_stat,"stat"},
//        {                        __NR_lstat,"lstat"},
//        {                      __NR_fcntl64,"fcntl64"},
//        {                     __NR_statfs64,"statfs64"},
//        {                    __NR_fstatfs64,"fstatfs64"},
//        {                   __NR_truncate64,"truncate64"},
//        {                  __NR_ftruncate64,"ftruncate64"},
//        {                       __NR_llseek,"llseek"},
//        {                   __NR_sendfile64,"sendfile64"},
//        {                    __NR_fstatat64,"fstatat64"},
//        {                      __NR_fstat64,"fstat64"},
//        {                        __NR_mmap2,"mmap2"},
//        {                 __NR_fadvise64_64,"fadvise64_64"},
//        {                       __NR_stat64,"stat64"},
//        {                      __NR_lstat64,"lstat64"},
};

DefineHookStub(pangle_syscall, void*, uint64_t code, void *k2, void *k3, void *k4, void *k5,
               void *k6, void *k7) {
    auto find = syscall2str.find(code - 233);
    uint64_t retAddr = (uint64_t) __builtin_return_address(0) - (uint64_t) targetLibBase;
    if (find != syscall2str.end()) {
        log2file("syscall: %s, ret: %p", find->second.c_str(), retAddr);
    } else {
        log2file("syscall: %s, ret: %p", find->second.c_str(), retAddr);
    }
    auto result = pHook_pangle_syscall(code, k2, k3, k4, k5, k6, k7);
    return result;
}


void hook_pangle_decode() {
    WhenSoInitHook(TargetLibName, [&](const string &path, void *addr, const string &funcType) {
        module_info_t info;
        hack_get_module_info(TargetLibName, &info);
        targetLibBase = (char *) info.module_address;
        InlineHookAddr(targetLibBase, 0x092CC, decode_str);
        InlineHookAddr(targetLibBase, 0x1043C, decode_str2);
        InlineHookAddr(targetLibBase, 0x2AAF8, hook_jump);
        InlineHookAddr(targetLibBase, 0x2AAAC, pangle_syscall);
    });
}
