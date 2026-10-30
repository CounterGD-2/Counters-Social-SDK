#include <jni.h>
#include <android/log.h>
#include <discordpp.h>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#define LOG_TAG "DiscordSocialBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
struct BridgeClient {
    std::unique_ptr<discordpp::Client> client;
    std::mutex mutex;
};
std::string fromJava(JNIEnv* env, jstring value) {
    if (!value) return {};
    const char* chars = env->GetStringUTFChars(value, nullptr);
    if (!chars) return {};
    std::string result(chars);
    env->ReleaseStringUTFChars(value, chars);
    return result;
}
BridgeClient* getClient(jlong handle) {
    return reinterpret_cast<BridgeClient*>(static_cast<intptr_t>(handle));
}
}

extern "C" JNIEXPORT jboolean JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeSdkAvailable(JNIEnv*, jclass) {
    return JNI_TRUE;
}

extern "C" JNIEXPORT jlong JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeCreate(
        JNIEnv* env, jclass, jstring applicationId) {
    try {
        const std::string id = fromJava(env, applicationId);
        if (id.empty()) return 0;
        auto* holder = new BridgeClient();
        holder->client = std::make_unique<discordpp::Client>();
        holder->client->SetApplicationId(static_cast<uint64_t>(std::stoull(id)));
        // Social SDK 1.10+ Android unauthenticated Rich Presence uses RPC.
        // No OAuth, UpdateToken, or Connect call is made in this mode.
        LOGI("Created Android Rich Presence client");
        return static_cast<jlong>(reinterpret_cast<intptr_t>(holder));
    } catch (const std::exception& error) {
        LOGE("Native initialization failed: %s", error.what());
        return 0;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeUpdatePresence(
        JNIEnv* env, jclass, jlong handle, jstring details, jstring state, jlong startEpochSeconds) {
    auto* holder = getClient(handle);
    if (!holder || !holder->client) return;
    std::lock_guard<std::mutex> lock(holder->mutex);

    discordpp::Activity activity{};
    activity.SetType(discordpp::ActivityTypes::Playing);
    const std::string detailsText = fromJava(env, details);
    const std::string stateText = fromJava(env, state);
    if (!detailsText.empty()) activity.SetDetails(detailsText);
    if (!stateText.empty()) activity.SetState(stateText);

    if (startEpochSeconds > 0) {
        discordpp::ActivityTimestamps timestamps{};
        timestamps.SetStart(static_cast<int64_t>(startEpochSeconds));
        activity.SetTimestamps(timestamps);
    }

    holder->client->UpdateRichPresence(std::move(activity), [](discordpp::ClientResult result) {
        if (result.Successful()) LOGI("Rich Presence update completed");
        else LOGE("Rich Presence update failed: %s", result.ToString().c_str());
    });
}

extern "C" JNIEXPORT void JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeClearPresence(
        JNIEnv*, jclass, jlong handle) {
    auto* holder = getClient(handle);
    if (!holder || !holder->client) return;
    std::lock_guard<std::mutex> lock(holder->mutex);
    holder->client->ClearRichPresence();
}

extern "C" JNIEXPORT void JNICALL
Java_dev_countergd_discordsocialbridge_SocialSdkBridge_nativeDestroy(
        JNIEnv*, jclass, jlong handle) {
    auto* holder = getClient(handle);
    if (!holder) return;
    {
        std::lock_guard<std::mutex> lock(holder->mutex);
        if (holder->client) {
            holder->client->ClearRichPresence();
            holder->client.reset();
        }
    }
    delete holder;
}
