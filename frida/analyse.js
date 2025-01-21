function inject_jnitrace(packet_name) {
    var so_path = ''
    if (Process.arch === "arm64") {
        so_path = "/data/libanalyse.so"
    } else if (Process.arch === "arm") {
        so_path = "/data/libanalyse.so"
    }

    let packet_name_addr = Memory.alloc(packet_name.length + 2)
    packet_name_addr.writeUtf8String(packet_name)

    Java.openClassFile('/data/frida_helper.dex').load();
    Java.use("com.fucker.gson.Gson")
    var helper = Java.use("com.frida.frida_helper")
    console.log(helper);

    var hook_so = Module.load(so_path)
    var init_hook = new NativeFunction(hook_so.findExportByName('init_jni_hook'), 'void', ['pointer', 'pointer'])
    let jnienv = Java.vm.getEnv().handle

    init_hook(jnienv, packet_name_addr)
    console.log("finish hook!")
}

 inject_jnitrace("")