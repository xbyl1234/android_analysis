package com.android_helper;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Main {
    static String source = "com.android.vending";// "com.android.packageinstaller";

    public static boolean install(String path) {
        if (path.endsWith(".apks")) {
            return installApks(path);
        } else {
            return installApk(path);
        }
    }

    public static boolean installApk(String path) {
        CommandUtil.RootCommandResult result = CommandUtil.RootCommand("pm install -i " + source + " " + path);
        return result.isSuccess();
    }

    public static boolean extractAPK(String apkPath, String outputDirectory, ArrayList<String> outFileName) {
        try (ZipInputStream zis = new ZipInputStream(Files.newInputStream(Paths.get(apkPath)))) {
            ZipEntry entry;
            while ((entry = zis.getNextEntry()) != null) {
                String entryName = entry.getName();
                outFileName.add(entryName);
                if (entryName.endsWith(".apk")) {
                    File outputFile = new File(outputDirectory + File.separator + entryName);
                    try (FileOutputStream fos = new FileOutputStream(outputFile)) {
                        byte[] buffer = new byte[1024];
                        int len;
                        while ((len = zis.read(buffer)) > 0) {
                            fos.write(buffer, 0, len);
                        }
                    }
                }
            }
            return true;
        } catch (IOException e) {
            System.out.println("extractAPK error: " + e);
            return false;
        }
    }

    public static boolean installApks(String path) {
        String dirPath = path.replace(".apks", "") + "/";
        CommandUtil.RootCommand("mkdir " + dirPath);
        ArrayList<String> outFile = new ArrayList<>();
        if (!extractAPK(path, dirPath, outFile)) {
            return false;
        }
        CommandUtil.RootCommand("chmod -R 777 " + dirPath);
        CommandUtil.RootCommandResult result = CommandUtil.RootCommand("pm install-create -i " + source);
        if (!result.isSuccess()) {
            System.out.println("error 4");
            return false;
        }
        String msg = result.message;
        String session = msg.substring(msg.indexOf("[") + 1, msg.indexOf("]"));
        for (String s : outFile) {
            result = CommandUtil.RootCommand("pm install-write " + session + " " + s + " " + dirPath + s);
            if (!result.isSuccess()) {
                System.out.println("error 5");
                return false;
            }
        }
        result = CommandUtil.RootCommand("pm install-commit " + session);
        return result.isSuccess();
    }

    public static void main(String[] args) {
        for (String item : args) {
            System.out.println("args: " + item);
        }
        String cmd = args[0];
        if (cmd.equals("ad")) {
        } else if (cmd.equals("install")) {
            if (install(args[1])) {
            System.out.println("install success");
        } else {
            System.out.println("install failed");
            }
        }
    }
}
