var fast_json

function init_gson() {
    Java.enumerateClassLoaders({
        onMatch: function (loader) {
            var old = Java.classFactory.loader
            Java.classFactory.loader = loader;
            try {
                fast_json = Java.use("com.plug.export.frida_helper")
            } catch (e) {
            }
            Java.classFactory.loader = old
        }, onComplete: function () {

        }
    });
    if (fast_json == null) {
        console.log("find frida helper error!")
    }
}

Java.perform(init_gson)

function object_2_string(object) {
    return fast_json.object_2_string(object)
}


function byte_2_hex_str(by) {
    return fast_json.byte_2_hex_str(by)
}


function byte_2_string(by, decode) {
    return fast_json.byte_2_string(by, decode)
}

