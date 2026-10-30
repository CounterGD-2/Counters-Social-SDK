#include <jni.h>
extern "C" JNIEXPORT jboolean JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeSdkAvailable(JNIEnv*, jclass) { return JNI_FALSE; }
extern "C" JNIEXPORT jlong JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeCreate(JNIEnv*, jclass, jstring) { return 0; }
extern "C" JNIEXPORT void JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeUpdatePresence(JNIEnv*, jclass, jlong, jstring, jstring, jlong) {}
extern "C" JNIEXPORT void JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeClearPresence(JNIEnv*, jclass, jlong) {}
extern "C" JNIEXPORT void JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeDestroy(JNIEnv*, jclass, jlong) {}
