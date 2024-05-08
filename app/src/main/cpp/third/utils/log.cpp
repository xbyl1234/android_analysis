#include "log.h"
#include "utils.h"

namespace xbyl {
    log defaultLog;

    void init_log(const string &tag, adapter *adapt) {
        if (adapt == nullptr) {
            defaultLog.enabled = false;
            return;
        }
        defaultLog.adapters.clear();
        defaultLog.set_adapt(adapt);
        defaultLog.setTag(tag);
    }

//   void init_log(const string &tag, vector<std::unique_ptr<adapter>> &adapt) {
//       defaultLog.adapters.clear();
//       defaultLog.adapters = std::move(adapt);
//       defaultLog.setTag(tag);
//   }

    void init_log(const string &tag) {
        defaultLog.setTag(tag);
    }

    void disable_adb_log() {
        for (const std::unique_ptr<adapter> &item: defaultLog.adapters) {
            if (item->type == log_adapt::use_adb) {
                item->enabled = false;
            }
        }
    }

    void enable_adb_log() {
        for (const std::unique_ptr<adapter> &item: defaultLog.adapters) {
            if (item->type == log_adapt::use_adb) {
                item->enabled = true;
            }
        }
    }
}
