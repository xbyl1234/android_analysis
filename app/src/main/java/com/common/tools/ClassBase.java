package com.common.tools;


import com.common.tools.hooker.HookTools;

public class ClassBase {
    public Class originClass = null;
    public Object originObject = null;

    public Object GetFieldValue(String name) {
        return HookTools.GetFieldValue(originClass, originObject, name);
    }

    public boolean SetFieldValue(String name, Object value) {
        return HookTools.SetField(originClass, originObject, name, value);
    }
}
