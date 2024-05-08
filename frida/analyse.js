var target_name = 'libnms.so'
var packet_name = 'com.dycxy.dat'

function inject_jnitrace() {
    var so_path = ''
    var hook_so
    if (Process.arch === "arm64") {
        // so_path = "/data/app/libnative_hook_64.so"
        so_path = "/data/app/libnative_hook.so"
    } else if (Process.arch === "arm") {
        so_path = "/data/app/libnative_hook_32.so"
    }

    hook_so = Module.load(so_path)
    console.log("jnitrace.so: " + so_path)

    let init_hook_addr = hook_so.findExportByName('init_jni_hook')
    var init_hook = new NativeFunction(init_hook_addr, 'void', ['pointer', 'pointer'])
    console.log("init_jni_hook: " + init_hook_addr)

    let use_file_log_addr = hook_so.findExportByName('use_file_log')
    var use_file_log = new NativeFunction(use_file_log_addr, 'void', ['pointer'])
    console.log("use_file_log: " + use_file_log_addr)

    let log_path = "/data/data/" + packet_name + "/jni_log.log"
    let log_path_addr = Memory.alloc(log_path.length + 2)
    log_path_addr.writeUtf8String(log_path)
    use_file_log(log_path_addr)

    let so_path_native_addr = Memory.alloc(target_name.length + 2)
    so_path_native_addr.writeUtf8String(target_name)
    console.log("target module: " + so_path_native_addr.readCString())
    let jnienv = Java.vm.getEnv().handle

    init_hook(jnienv, so_path_native_addr)
    console.log("finish hook!")
}

function only_watch() {

    var so_path = ''
    var hook_so
    if (Process.arch === "arm64") {
        // so_path = "/data/app/libnative_hook_64.so"
        so_path = "/data/app/libnative_hook.so"
    } else if (Process.arch === "arm") {
        so_path = "/data/app/libnative_hook_32.so"
    }

    hook_so = Module.load(so_path)
    console.log("jnitrace.so: " + so_path)

    let init_only_watch_addr = hook_so.findExportByName('init_only_watch')
    var init_only_watch = new NativeFunction(init_only_watch_addr, 'void', ['pointer', 'pointer'])
    console.log("init_jni_hook: " + init_only_watch_addr)

    let use_file_log_addr = hook_so.findExportByName('use_file_log')
    var use_file_log = new NativeFunction(use_file_log_addr, 'void', ['pointer'])
    console.log("use_file_log: " + use_file_log_addr)

    let log_path = "/data/data/" + packet_name + "/str_log.log"
    let log_path_addr = Memory.alloc(log_path.length + 2)
    log_path_addr.writeUtf8String(log_path)
    // use_file_log(log_path_addr)


    let so_path_native_addr = Memory.alloc(target_name.length + 2)
    so_path_native_addr.writeUtf8String(target_name)
    console.log("target module: " + so_path_native_addr.readCString())
    let jnienv = Java.vm.getEnv().handle
    init_only_watch(jnienv, so_path_native_addr)
    console.log("finish hook!")
}


function hook_android_dlopen_ext(modeul_name, callback) {
    var android_dlopen_ext = Module.findExportByName(null, "android_dlopen_ext")
    console.log(" android_dlopen_ext:" + android_dlopen_ext);
    Interceptor.attach(android_dlopen_ext, {
        onEnter: function (args) {
            this.file_name = args[0].readCString();
        },
        onLeave: function (retval) {
            if (this.file_name.indexOf(modeul_name) >= 0) {
                console.log("loaded : " + this.file_name)
                callback()
            }
        },
    })
}

// inject_jnitrace()
// frida -U -f com.whatsapp -l analyse.js --no-pause
// frida -U -f org.sugram.lite -l analyse.js --no-pause
//frida -U -f com.view.asim -l analyse.js --no-pause

hook_android_dlopen_ext(target_name, only_watch)