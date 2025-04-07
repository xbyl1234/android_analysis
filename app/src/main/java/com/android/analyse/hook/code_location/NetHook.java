package com.android.analyse.hook.code_location;

import java.lang.reflect.Constructor;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.channels.SocketChannel;

import javax.net.ssl.SSLSocket;

public class NetHook {

    static public void Hook(ClassLoader classLoader) {
//        public static InetAddress InetAddress.getByName(String host)
//
//// 获取所有 IP 地址
//        public static InetAddress[] InetAddress.getAllByName(String host)
//
//// 示例
//        InetAddress[] addresses = InetAddress.getAllByName("www.google.com");
//        for (InetAddress addr : addresses) {
//            Log.d("DNS", "IP: " + addr.getHostAddress());
//        }
//
//        Network network = connectivityManager.getActiveNetwork();
//        InetAddress[] addresses = network.getAllByName("cdn.provider.com");
//
//// 使用 LinkProperties（需要权限）
//        LinkProperties linkProp = connectivityManager.getLinkProperties(network);
//        List<InetAddress> dnsServers = linkProp.getDnsServers();
//        InetAddress addr = InetAddress.getByName("142.250.189.164");
//        String hostname = addr.getHostName(); // 可能返回 IP 本身
//
//// 强制反向解析
//        String canonicalHost = addr.getCanonicalHostName();
//        // 使用 Network 进行反向解析
//        String reverseDns = network.getByName(ipAddress).getHostName();
//
//// 使用 DnsResolver（API 级别 29+）
//        DnsResolver resolver = DnsResolver.getInstance();
//        resolver.reverseLookup(inetAddress, executor, new DnsResolver.Callback<String>() {
//            @Override
//            public void onAnswer(String hostname, int ttl) {
//                // 处理结果
//            }
//        });
    }
}
