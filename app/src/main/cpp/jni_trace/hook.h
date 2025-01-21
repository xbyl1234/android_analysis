#pragma once

#include <jni.h>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <dlfcn.h>

//#include <unwindstack/LocalUnwinder.h>
#include "../third/byopen/hack_dlopen.h"
#include "../third/utils/jni_helper.hpp"
#include "../third/utils/linux_helper.h"
#include "../third/utils/utils.h"
#include "../third/utils/log.h"
#include "../base/when_hook.h"
#include "../base/hook.h"
#include "../third/log2file/app_file_writer.h"

#include "parse_java_sig.h"
#include "jni_helper.h"
#include "jni_treace.h"
#include "format.h"

using namespace format;