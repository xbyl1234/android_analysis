#include <string>
#include "../third/sqlite/sqlite3.h"
#include "../third/utils/log.h"
#include "../third/utils/utils.h"
#include "../third/utils/linux_helper.h"

using namespace std;

int main() {
    xbyl::init_log("daily", new xbyl::adapter_printf());
    while (true) {
        string result = RunCmd("ps -A|grep com.daily.featured.opoper");
        if (result.length() > 5) {
            logi("app running");
            sleep(60 * 2);
//            sleep(2);
            logi("kill app");
            RunCmd("pkill -f com.daily.featured.opoper");
        } else {
            logi("app not running");
            sleep(2);
        }
    }
    return 0;
}