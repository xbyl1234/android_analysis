package com.common;

import java.util.List;
import java.util.Random;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.UUID;

public class random {
    static String char_set_abc = "abcdefghijklmnopqrstuvwxyz";
    static String char_set_ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static String char_set_123 = "0123456789";
    static String char_set_16_num = "0123456789abcdef";
    static String char_set_all = char_set_abc + char_set_ABC + char_set_123;
    static Random random = new Random(System.currentTimeMillis());

    public static boolean getProbabilisticBoolean(int percentage) {
        int randomValue = random.nextInt(101);
        return randomValue <= percentage;
    }

    public static boolean random_bool() {
        return random_int(1, 2) == 1;
    }

    public static int random_int(int min, int max) {
        return (int) (min + random.nextDouble() * (max - min + 1));
    }

    public static double random_double(double min, double max) {
        return random.nextDouble() * (max - min) + min;
    }

    public static String random_string(String char_set, int len) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < len; i++) {
            sb.append(char_set.charAt(random_int(0, char_set.length() - 1)));
        }
        return sb.toString();
    }

    public static <T> T chose_one(T[] data) {
        return data[(random_int(0, data.length - 1))];
    }

    public static <T> T chose_one(List<T> data) {
        return data.get(random_int(0, data.size() - 1));
    }

    public static String gen_uuid() {
        return UUID.randomUUID().toString();
    }

    public static String gen_md5() {
        return random_string(char_set_16_num, 32);
    }

    public static class WeightRandom<T> {
        private final TreeMap<Integer, T> weightMap;

        public WeightRandom() {
            weightMap = new TreeMap<>();
        }

        public void add(T obj, int weight) {
            if (weight > 0) {
                int lastWeight = (this.weightMap.size() == 0) ? 0 : this.weightMap.lastKey();
                this.weightMap.put(weight + lastWeight, obj);// 权重累加
            }
        }

        public void clear() {
            this.weightMap.clear();
        }

        public T next() {
            int randomWeight = (int) (this.weightMap.lastKey() * Math.random());
            SortedMap<Integer, T> tailMap = this.weightMap.tailMap(randomWeight, false);
            return this.weightMap.get(tailMap.firstKey());
        }
    }
}


