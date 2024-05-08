package com.android_helper;

import android.util.Log;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStreamReader;

public class CommandUtil {
    private static final String TAG = "Adapter";

    public static RootCommandResult RootCommand(String command) {
        RootCommandResult result = new RootCommandResult();
        Process process = null;
        DataOutputStream dataOutputStream = null;
        DataInputStream dataInputStream = null;
        StringBuffer resultBuffer = new StringBuffer();
        try {
            process = Runtime.getRuntime().exec("su");
            dataOutputStream = new DataOutputStream(process.getOutputStream());
            dataInputStream = new DataInputStream(process.getInputStream());
            dataOutputStream.writeBytes(command + "\n");
            dataOutputStream.writeBytes("exit\n");
            dataOutputStream.flush();
            InputStreamReader inputStreamReader = new InputStreamReader(
                    dataInputStream, "UTF-8");
            BufferedReader bufferedReader = new BufferedReader(
                    inputStreamReader);
            String line = null;
            while ((line = bufferedReader.readLine()) != null) {
                resultBuffer.append(line);
                resultBuffer.append("\n");
            }
            bufferedReader.close();
            inputStreamReader.close();
            process.waitFor();
            result.code = process.exitValue();
            result.message = resultBuffer.toString();
            Log.d(TAG, "root command : " + command + " ,result :" + result.message + " ,exit value : " + result.code);
        } catch (Exception e) {
            Log.e(TAG, "root command with result error : " + e.getMessage(), e);
            result.code = 1;
            result.message = e.getMessage();
        } finally {
            try {
                if (dataOutputStream != null) {
                    dataOutputStream.close();
                }
                if (dataInputStream != null) {
                    dataInputStream.close();
                }
                process.destroy();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return result;
    }

    public static class RootCommandResult {
        public int code;
        public String message;

        public boolean isSuccess() {
            //0成功1失败2未知情况
            return code == 0;
        }
    }
}