chinglish document

## how to build

 ```
 android studio build , must debug, some bug in release that dobby dont work 
 
 tools lib will in app/libs/arm64-v8a
 
 libnative_hook.so : trace jni
 libssl2.so : hook ssl key log 
 sniffer.so : use tcpdum to capture specified app packet, must put it and tcpdump on /data
```
 
## how to use and function

```
1. frida printf help class
    project/frida/frida_helper.js can help you convert object to string,using inject fastjson and gjson by xposed. 
```

```
2. capture a app packet
  put sniffer.so and tcpdump on /data
  use cmd: ./sniffer.so com.xxxx pacp_name , and ctrl-c stop capture , save to /sdcard/com.xxxx_pacp_name.pcap
```

```
3. hook ssl key log
  inject libssl2.so by yourself, packet.txt will write in /sdcard/Android/com.xxxxx/packet.txt, need some sdcard permission
```


```
4. trace all java2c, c2java jni call log
  put libnative_hook.so  in /data/app , and chmod 777,  use project/frida/analyse.js to inject it in your target app
  and do this before:
  write your target packet name on packet_name
  write your target trace lib name on target_name
  
  use use_file_log() can write log in file ,if not will wirte in android log
  must enable frida printf help class, or undefine USE_IN_JAVA_PARSE
  other:
  arm32 is bug, fix bug by yourself
```

wecome to give me money, qq is 734248537, see you leite 

 
2025/4/8
jni trace look at cpp/jnitrace
object format look at frida_helper




