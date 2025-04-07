package com.android.analyse;

import android.content.Context;
import android.widget.Toast;

import com.common.utils;

public class FileManger {
    static void GetAssetsFile(Context context, String name) throws Exception {
        byte[] data = utils.ReadFromAssets(context, name);
        String dataPath = "/data/data/" + context.getApplicationInfo().packageName + "/" + name;
        utils.save_file(dataPath, data);
    }

    static void CpyFile(String dir, String name, String target) throws Exception {
        String targetPath = target + "/" + name;
        utils.RunShell("rm -f " + targetPath);
        utils.RunShell("cp " + dir + "/" + name + " " + targetPath);
        utils.RunShell("chmod 777 " + targetPath);
        utils.RunShell("chcon u:object_r:system_lib_file:s0 " + targetPath);
    }

    static public void CpyFile(Context context) {
        try {
            CpyFile(context.getApplicationInfo().nativeLibraryDir, "libanalyse.so", "/data");
            String appPath = "/data/data/" + context.getApplicationInfo().packageName;
            GetAssetsFile(context, "frida_helper.dex");
//            GetAssetsFile(context, "android_helper.dex");
            GetAssetsFile(context, "sniffer");
            GetAssetsFile(context, "tcpdump");
            CpyFile(appPath, "frida_helper.dex", "/data");
//            CpyFile(appPath, "android_helper.dex", "/data");
            CpyFile(appPath, "sniffer", "/data");
            CpyFile(appPath, "tcpdump", "/data");
            Toast.makeText(context, "复制成功", Toast.LENGTH_LONG).show();
        } catch (Throwable e) {
            Toast.makeText(context, "复制失败: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

}
