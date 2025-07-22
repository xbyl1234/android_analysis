#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <fstream>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/uio.h>

#include "utils.h"
#include "linux_helper.h"

using std::stringstream;

string get_packet_name() {
    string cmdLine = GetCmdLine(getpid());
    int p = cmdLine.find(":");
    if (p >= 0) {
        return cmdLine.substr(0, p);
    }
    return cmdLine;
}

string mid_string(const string &src, const string &start, const string &end) {
    int p = src.find(start);
    if (p == -1) {
        return "";
    }
    int pe = src.find(end, p + start.size());
    if (pe == -1) {
        return "";
    }
    return src.substr(p + start.size(), pe - p - start.size());
}

string get_uuid() {
    char uuid[37] = {0};
    int fd = open("/proc/sys/kernel/random/uuid", O_RDONLY);
    if (fd >= 0) {
        read(fd, uuid + 1, 36);
    }
    close(fd);
    return uuid;
}

int64_t get_time() {
    time_t timep;
    time(&timep);
    return timep;
}

string time_to_string(int64_t tick) {
    tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &tick);
#else
    localtime_r((time_t *) &tick, &tm);
#endif

    std::stringstream stm;
    stm << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return stm.str();
}

int64_t string_to_time(const string &time_str, const string &fmt = "%Y-%m-%d %H:%M:%S") {
    tm tm{};
    std::stringstream stm;
    stm << time_str;
    stm >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return mktime(&tm);
}

bool WritFile(const string &path, const char *buf, int len) {
    std::fstream file(path.c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(buf, len);
    file.flush();
    file.close();
    return true;
}

char *hex2str(const char *hex, int hex_len, char *str, int buf_len) {
    const char *cHex = "0123456789ABCDEF";
    int i = 0;
    if (hex_len * 2 >= buf_len) {
        hex_len = buf_len / 2 - 1;
    }
    if (hex_len <= 0) {
        str[0] = 0;
        return nullptr;
    }
    for (int j = 0; j < hex_len; j++) {
        auto a = (unsigned int) hex[j];
        str[i++] = cHex[(a & 0xf0) >> 4];
        str[i++] = cHex[(a & 0x0f)];
//        if ((j + 1) % 16 == 0) {
//            str[i++] = '\n';
//        } else {
//            str[i++] = ' ';
//        }
    }
    str[i] = '\0';
    return str;
}

int hex2int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

char *str2hex(const char *str, int str_len, char *hex, int buf_len) {
    if (str_len / 2 > buf_len) {
        str_len = buf_len * 2;
    }
    for (int i = 0; i < str_len; i += 2) {
        int high = hex2int(str[i]);
        int low = hex2int(str[i + 1]);
        hex[i / 2] = (high << 4) | low;
    }
    return hex;
}

//字符串分割函数
vector<string> string_split(const string &str, const string &pattern) {
    string::size_type pos;
    vector<string> result;
    int size = str.size();

    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos == string::npos) {
            string s = str.substr(i);
            result.push_back(s);
            break;
        }
        if (pos < size) {
            string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

//vector<string> string_split(const string &str, const string &pattern) {
//    vector<string> ret;
//    if (pattern.empty()) return ret;
//    size_t start = 0, index = str.find_first_of(pattern, 0);
//    while (index != std::string::npos) {
//        if (start != index)
//            ret.push_back(str.substr(start, index - start));
//        start = index + 1;
//        index = str.find_first_of(pattern, start);
//    }
//    if (!str.substr(start).empty())
//        ret.push_back(str.substr(start));
//    return ret;
//}

string replace_all(const string &str, const string &old_value, const string &new_value) {
    string cp = str;
    while (true) {
        string::size_type pos(0);
        if ((pos = cp.find(old_value)) != string::npos)
            cp.replace(pos, old_value.length(), new_value);
        else break;
    }
    return cp;
}

struct AutoInitRand {
    AutoInitRand() {
        srand(time(NULL));
    }
} _;

int gen_number(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

float gen_double(float min, float max) {
    return (rand() / (float) RAND_MAX) * (max - min) + min;
}


const char *CharSet_123 = "0123456789";
const char *CharSet_ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *CharSet_abc = "abcdefghijklmnopqrstuvwxyz";
const char *CharSet_hex = "0123456789abcdef";
const char *CharSet_ABC123 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const char *CharSet_all = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void gen_hex(int len, char *result) {
    for (int i = 0; i < len; ++i) {
        result[i] = gen_number(0, 0xff);
    }
}

string gen_str(const char *charSet, int maxIdx, int len) {
    string buff;
    buff.reserve(len + 1);
    for (int i = 0; i < len; ++i) {
        buff += charSet[gen_number(0, maxIdx)];
    }
    return buff;
}

string gen_strABC123(int len) {
    return gen_str(CharSet_ABC123, 35, len);
}

string gen_hexstr(int len) {
    return gen_str(CharSet_hex, 15, len);
}

string gen_strabc(int len) {
    return gen_str(CharSet_abc, 25, len);
}

string gen_strABC(int len) {
    return gen_str(CharSet_ABC, 25, len);
}

string gen_str123(int len) {
    return gen_str(CharSet_123, 9, len);
}

string gen_strall(int len) {
    return gen_str(CharSet_all, 61, len);
}

string gen_str(const char *charSet, int len) {
    return gen_str(charSet, strlen(charSet) - 1, len);
}

string gen_uuid() {
    return gen_hexstr(8) + "-" +
           gen_hexstr(4) + "-" +
           gen_hexstr(4) + "-" +
           gen_hexstr(4) + "-" +
           gen_hexstr(12);
}

string to_lower(const string &str) {
    string cp = str;
    transform(cp.begin(), cp.end(), cp.begin(),
              [](unsigned char c) {
                  return std::tolower(c);
              });
    return cp;
}

string to_upper(const string &str) {
    string cp = str;
    transform(cp.begin(), cp.end(), cp.begin(),
              [](unsigned char c) {
                  return std::toupper(c);
              });
    return cp;
}


bool ReadFile(const string &path, char **data, int *len) {
    if (access(path.c_str(), R_OK) != 0) {
//        loge("read file %s no read permission", path.c_str());
        return false;
    }
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) {
//        loge("read file %s open failed", path.c_str());
        return false;
    }
    ifs.seekg(0, ifs.end);
    auto fos = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    char *buff = new char[fos];
    ifs.read(buff, fos);
    ifs.close();
    *data = buff;
    *len = fos;
    return true;
}

string ReadFile(const string &path) {
    if (access(path.c_str(), R_OK) != 0) {
//        loge("read file %s no read permission", path.c_str());
        return "";
    }
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
//        loge("read file %s open failed", path.c_str());
        return "";
    }
    stringstream buffer;
    buffer << ifs.rdbuf();
    ifs.close();
    return buffer.str();
}


void StringAppendV(std::string *dst, const char *format, va_list ap) {
    // First try with a small fixed size buffer
    char *space = new char[1024]{};
    defer([&]() {
        delete[]space;
    });
    // It's possible for methods that use a va_list to invalidate
    // the data in it upon use.  The fix is to make a copy
    // of the structure before using it and use that copy instead.
    va_list backup_ap;
    va_copy(backup_ap, ap);
    int result = vsnprintf(space, 1024, format, backup_ap);
    va_end(backup_ap);

    if (result < static_cast<int>(sizeof(space))) {
        if (result >= 0) {
            // Normal case -- everything fit.
            dst->append(space, result);
            return;
        }

        if (result < 0) {
            // Just an error.
            return;
        }
    }

    // Increase the buffer size to the size requested by vsnprintf,
    // plus one for the closing \0.
    int length = result + 1;
    char *buf = new char[length];

    // Restore the va_list before we use it again
    va_copy(backup_ap, ap);
    result = vsnprintf(buf, length, format, backup_ap);
    va_end(backup_ap);

    if (result >= 0 && result < length) {
        // It fit
        dst->append(buf, result);
    }
    delete[] buf;
}


namespace xbyl {
    std::string format_string(const char *fmt, va_list ap) {
        std::string result;
        StringAppendV(&result, fmt, ap);
        return result;
    }

    std::string format_string(const string fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        std::string result;
        StringAppendV(&result, fmt.c_str(), ap);
        va_end(ap);
        return result;
    }
}

std::string StringPrintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    std::string result;
    StringAppendV(&result, fmt, ap);
    va_end(ap);
    return result;
}

long get_system_time_nanosecond() {
    struct timespec timestamp = {};
    if (0 == clock_gettime(CLOCK_REALTIME, &timestamp))
        return timestamp.tv_sec * 1000000000 + timestamp.tv_nsec;
    else
        return 0;
}

long get_system_time_microsecond() {
    struct timeval timestamp = {};
    if (0 == gettimeofday(&timestamp, NULL))
        return timestamp.tv_sec * 1000000 + timestamp.tv_usec;
    else
        return 0;
}

long get_system_time_millisecond() {
    struct timeval timestamp = {};
    if (0 == gettimeofday(&timestamp, NULL))
        return timestamp.tv_sec * 1000 + timestamp.tv_usec / 1000;
    else
        return 0;
}

int
TraversePackagesList(std::function<bool(const vector<string> &)> callback) {
    const string &data = ReadFile("/data/system/packages.list");
    if (data.empty()) {
        return -1;
    }

    const auto &lines = string_split(data, "\n");
    for (const auto &line: lines) {
        const auto &sp = string_split(line, " ");
        if (!callback(sp)) {
            return 1;
        }
    }
    return 0;
}

bool ReadAllPackagesGid(vector<int> &gids) {
    return TraversePackagesList([&](const vector<string> &line_sp) -> bool {
        if (line_sp.size() >= 2) {
            int gid = stoi(line_sp[1]);
            if (gid >= 10000) {
                gids.push_back(gid);
            }
        }
        return true;
    }) != -1;
}


int ReadPkgGid(const string &pkgName) {
    int ret = -1;
    TraversePackagesList([&](const vector<string> &line_sp) -> bool {
        if (line_sp.size() < 5) {
            return true;
        }
        if (line_sp[0] == pkgName) {
            ret = stoi(line_sp[1]);
            return false;
        }
        return true;
    });
    return ret;
}

string ReadPkgDirSelinuxCtx(const string &pkgName) {
    string ret = "";
    TraversePackagesList([&](const vector<string> &line_sp) -> bool {
        if (line_sp.size() < 5) {
            return true;
        }
        if (line_sp[0] == pkgName) {
            ret = line_sp[4].empty() ? "" : line_sp[4] + ":complete";
            return false;
        }
        return true;
    });
    return ret;
}

vector<Stack> GetStackInfo(int num, ...) {
    vector<Stack> frame;
    va_list args;
    va_start(args, num);
    for (int i = 0; i < num; ++i) {
        void *addr = va_arg(args, void*);
        Dl_info info{};

        if (dladdr(addr, &info) != 0) {
            frame.push_back({
                                    info.dli_fname,
                                    (void *) ((uint64_t) addr - (uint64_t) info.dli_fbase)
                            });
        } else {
            frame.push_back({
                                    "unknow",
                                    (void *) ((uint64_t) addr - (uint64_t) info.dli_fbase)
                            });
        }
    }
    va_end(args);
    return frame;
}

bool check_mem(void *p) {
    int pageSize = getpagesize();
    unsigned char vec = 0;
    uint64_t start = ((uint64_t) p) & (~(pageSize - 1));
    int result = mincore((void *) start, pageSize, &vec);
    return result == 0 && vec == 1;
}

extern "C" bool check_stack(void *p) {
    if (!check_mem(p)) {
        return false;
    }
    if (!check_mem((void *) *((uint64_t *) p))) {
        return false;
    }
    if (!check_mem((void *) (((uint64_t) p) + 8))) {
        return false;
    }
    if (!check_mem((void *) *((uint64_t *) (((uint64_t) p) + 8)))) {
        return false;
    }
    return true;
}

string stack2str(const vector<Stack> &stack) {
    string result;
    for (const Stack &item: stack) {
        result += xbyl::format_string("%s:%p,", item.name.c_str(), item.offset);
    }
    return result;
}

string hexdump_memory(const uint8_t *data, size_t size, uint64_t address) {
    size_t offset = 0;
    string result;
    while (offset < size) {
        result += xbyl::format_string("%08llx: ", address + offset);
        std::string ascii;
        for (size_t i = 0; i < 16; ++i) {
            if (offset + i < size) {
                uint8_t byte = data[offset + i];
                result += xbyl::format_string("%02x ", byte);
                ascii += (std::isprint(byte) ? static_cast<char>(byte) : '.');
            } else {
                result += "   ";
                ascii += " ";
            }
            if (i == 7)
                result += " ";
        }

        result += " |";
        result += ascii;
        result += "|";
        result += "\n";
        offset += 16;
    }
    return result;
}

bool is_ascii_string(const uint8_t *data, size_t length) {
    if (data == nullptr) {
        return false;
    }
    bool hasNonSpaceChar = false;
    for (size_t i = 0; i < length; ++i) {
        if (data[i] == '\0') {
            if (i == 0) {
                return false;
            }
            return hasNonSpaceChar;
        }
        if (data[i] < 0x20 || data[i] > 0x7E) {
            return false;
        }
        if (data[i] != ' ') {
            hasNonSpaceChar = true;
        }
    }
    return hasNonSpaceChar;
}

bool safe_read_memory(uint64_t address, uint8_t *buffer, size_t length) {
    struct iovec local_iov;
    struct iovec remote_iov;
    local_iov.iov_base = buffer;
    local_iov.iov_len = length;
    remote_iov.iov_base = reinterpret_cast<void *>(address);
    remote_iov.iov_len = length;
    ssize_t nread = process_vm_readv(getpid(), &local_iov, 1, &remote_iov, 1, 0);
    if (nread == static_cast<ssize_t>(length)) {
        return true;
    } else {
        return false;
    }
}