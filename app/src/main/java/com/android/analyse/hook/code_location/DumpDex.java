package com.android.analyse.hook.code_location;

import android.util.Log;

import com.common.log;
import com.common.tools.hooker.HookTools;

import java.io.File;
import java.io.FileOutputStream;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.nio.file.Files;

import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexFile;
import dalvik.system.InMemoryDexClassLoader;
import dalvik.system.PathClassLoader;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedBridge;
import de.robv.android.xposed.XposedHelpers;

public class DumpDex {
    private static final String DEX_SAVE_PATH = "/data/data/%s/dex/";

    private static void saveDexFile(String dexPath, String saveDir) {
        try {
            File src = new File(dexPath);
            if (!src.exists()) return;
            String fileName = "dex_" + System.currentTimeMillis() + ".dex";
            File dest = new File(saveDir, fileName);
            Files.copy(src.toPath(), dest.toPath());
            XposedBridge.log("DEX saved to: " + dest.getAbsolutePath());
        } catch (Exception e) {
            XposedBridge.log("Save DEX error: " + e.getMessage());
        }
    }

    private static void saveByteBufferDex(ByteBuffer[] buffers, String saveDir) {
        try {
            for (int i = 0; i < buffers.length; i++) {
                ByteBuffer buffer = buffers[i].duplicate();
                byte[] data = new byte[buffer.remaining()];
                buffer.get(data);
                String fileName = "memory_" + System.currentTimeMillis() + "_" + i + ".dex";
                File dest = new File(saveDir, fileName);
                try (FileOutputStream fos = new FileOutputStream(dest)) {
                    fos.write(data);
                }
                XposedBridge.log("Memory DEX saved to: " + dest.getAbsolutePath());
            }
        } catch (Exception e) {
            XposedBridge.log("Save memory DEX error: " + e.getMessage());
        }
    }

    static public void Hook(ClassLoader classLoader, String pkgName) {
        final String savePath = String.format(DEX_SAVE_PATH, pkgName);
        try {
            new File(savePath).mkdirs();
            log.i("savePath " + savePath);
        } catch (Throwable e) {
        }

        XposedBridge.hookAllConstructors(BaseDexClassLoader.class, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
                if (param.args[0].getClass() == String.class) {
                    String dexPath = (String) param.args[0];
                    log.i("BaseDexClassLoader path " + dexPath + ", " + Log.getStackTraceString(new Throwable()));
                    saveDexFile(dexPath, savePath);
                } else {
                    ByteBuffer[] dexFiles = (ByteBuffer[]) param.args[0];
                    log.i("BaseDexClassLoader " + dexFiles.length + ", " + Log.getStackTraceString(new Throwable()));
                    saveByteBufferDex(dexFiles, savePath);
                }
            }
        });

        Class Element = XposedHelpers.findClass("dalvik.system.DexPathList$Element", classLoader);
        Class Elements = Array.newInstance(Element, 0).getClass();

        XposedHelpers.findAndHookMethod(DexFile.class, "openDexFileNative",
                String.class, String.class, int.class, ClassLoader.class, Elements,
                new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        String dexPath = (String) param.args[0];
                        log.i("openDexFileNative " + dexPath + ", " + Log.getStackTraceString(new Throwable()));
                        saveDexFile(dexPath, savePath);
                    }
                });
        XposedHelpers.findAndHookMethod(DexFile.class, "openInMemoryDexFilesNative", ByteBuffer[].class, byte[][].class,
                int[].class, int[].class, ClassLoader.class, Elements, new XC_MethodHook() {
                    @Override
                    protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                        super.beforeHookedMethod(param);
                        ByteBuffer[] dexBuffers = (ByteBuffer[]) param.args[0];
                        log.i("openInMemoryDexFilesNative " + dexBuffers.length + ", " + Log.getStackTraceString(new Throwable()));
                        saveByteBufferDex(dexBuffers, savePath);
                    }
                });
    }
}
