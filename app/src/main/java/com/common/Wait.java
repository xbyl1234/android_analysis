package com.common;

public class Wait {
    volatile boolean finish = false;

    public void Done() {
        finish = true;
    }

    public boolean WaitFor() {
        while (!finish) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException ignored) {
            }
        }
        return finish;
    }

    public boolean WaitFor(long mil) {
        long start = System.currentTimeMillis();
        while (!finish && (System.currentTimeMillis() - start) < mil) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException ignored) {
            }
        }
        return finish;
    }

}
