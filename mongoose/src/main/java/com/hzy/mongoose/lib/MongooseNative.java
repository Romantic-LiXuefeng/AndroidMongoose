package com.hzy.mongoose.lib;

public class MongooseNative {

    /**
     * Get the mongoose version information
     *
     * @return version info String
     */
    public static native String getVersionInfo();

    /**
     * Start the server
     *
     * @param port     the server port
     * @param rootPath the root path
     * @return 0 if succeed
     */
    public static native int nStartServer(String port, String rootPath);

    /**
     * Stop the server
     *
     * @return 0 if succeed
     */
    public static native int nStopServer();

    static {
        System.loadLibrary("mongoose");
    }
}
