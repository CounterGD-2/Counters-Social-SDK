package dev.countergd.discordsocialbridge;

/** JNI facade for Android unauthenticated Rich Presence. */
public final class SocialSdkBridge implements AutoCloseable {
    private static final boolean NATIVE_LOADED;
    static {
        boolean loaded;
        try {
            System.loadLibrary("discord_social_bridge");
            loaded = true;
        } catch (UnsatisfiedLinkError error) {
            loaded = false;
        }
        NATIVE_LOADED = loaded;
    }

    private long nativeHandle;

    public static boolean isNativeSdkAvailable() {
        return NATIVE_LOADED && nativeSdkAvailable();
    }

    public synchronized void initialize(String applicationId) {
        if (!isNativeSdkAvailable()) {
            throw new IllegalStateException("Official Discord Social SDK native library is unavailable");
        }
        if (nativeHandle != 0L) throw new IllegalStateException("Already initialized");
        if (applicationId == null || !applicationId.matches("[0-9]{17,20}")) {
            throw new IllegalArgumentException("Expected a numeric Discord Application ID");
        }
        nativeHandle = nativeCreate(applicationId);
        if (nativeHandle == 0L) throw new IllegalStateException("Native SDK initialization failed");
    }

    public synchronized void updatePresence(String details, String state, long startEpochSeconds) {
        requireInitialized();
        nativeUpdatePresence(nativeHandle, details == null ? "" : details,
                state == null ? "" : state, Math.max(0L, startEpochSeconds));
    }

    public synchronized void clearPresence() {
        if (nativeHandle != 0L) nativeClearPresence(nativeHandle);
    }

    @Override public synchronized void close() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle);
            nativeHandle = 0L;
        }
    }

    private void requireInitialized() {
        if (nativeHandle == 0L) throw new IllegalStateException("Call initialize() first");
    }

    private static native boolean nativeSdkAvailable();
    private static native long nativeCreate(String applicationId);
    private static native void nativeUpdatePresence(long handle, String details, String state, long startEpochSeconds);
    private static native void nativeClearPresence(long handle);
    private static native void nativeDestroy(long handle);
}
