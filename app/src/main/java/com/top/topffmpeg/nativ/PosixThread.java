package com.top.topffmpeg;

/**
 * Description:PosixThread
 *
 * @author lihaitao
 * @version 1.0
 * @since 2019/5/20 2:27 PM
 */
public class PosixThread {
    static {
        System.loadLibrary("ffmpeg-main");
    }
    public static native void posix_init();
    public static native void posix_destroy();
    public static native void posix_getuuid();
}
