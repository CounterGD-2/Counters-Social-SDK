# Discord Social SDK Android Bridge

JNI-backed Android library for publishing Rich Presence to the signed-in Discord Android app using Discord Social SDK 1.10+ unauthenticated RPC.

## Status
- Java/JNI API: implemented.
- Native adapter: implemented against documented `discordpp::Client`, `discordpp::Activity`, and `UpdateRichPresence`.
- SDK binaries: NOT included. Obtain the official proprietary SDK package through the Discord Developer Portal.
- Device verification: not yet performed. CI without the SDK builds a stub; it does not prove Discord connectivity.

## Requirements
- Android API 24+, Android NDK, CMake
- JDK 17, Gradle 8.11.1, Android Gradle Plugin 8.9.1
- Discord Social SDK 1.10+ and a Discord Application ID

## Install the official SDK
Download the C++/Android package from your Discord Developer Portal and extract it to `third_party/discord_social_sdk/`. It must contain `include/discordpp.h` and the Android `libdiscord_partner_sdk.so` for `arm64-v8a`. Archive layouts can vary; if auto-detection fails, pass the exact library path using `-PDISCORD_SDK_LIBRARY=/absolute/path/libdiscord_partner_sdk.so` (see `native/CMakeLists.txt`).

The SDK is proprietary and is intentionally excluded from this repository (`third_party/` is gitignored). Do not commit SDK binaries, OAuth tokens, refresh tokens, bot tokens, or client secrets.

## Build
Without SDK files, this produces a non-functional stub AAR:
```bash
gradle :bridge:assembleDebug --no-daemon
```
With the SDK extracted:
```bash
gradle :bridge:assembleDebug --no-daemon \
  -PdiscordSdkRoot="$PWD/third_party/discord_social_sdk" \
  -PDISCORD_SDK_LIBRARY="/absolute/path/to/libdiscord_partner_sdk.so"
```
The default ABI is `arm64-v8a`.

## Java API
```java
SocialSdkBridge bridge = new SocialSdkBridge();
if (!SocialSdkBridge.isNativeSdkAvailable()) {
    throw new IllegalStateException("Official Discord SDK is not packaged");
}
bridge.initialize("YOUR_DISCORD_APPLICATION_ID");
bridge.updatePresence(
    "Exploring a survival world",
    "Minecraft Java 26.2",
    System.currentTimeMillis() / 1000L
);
// On shutdown:
bridge.clearPresence();
bridge.close();
```
The application ID is a public numeric identifier, not a token. No OAuth or user credentials are used by this Rich Presence-only transport.

## Integration constraints
- Android Social SDK 1.10 added Rich Presence via unauthenticated RPC. This implementation sets the application ID and calls `UpdateRichPresence`; it intentionally does not call `Authorize`, `UpdateToken`, or `Connect`.
- This library does not implement OAuth/account linking, friends, DMs, lobbies, voice, or desktop IPC.
- A normal NeoForge mod JAR cannot install native code into Android. The host Android launcher/application must package this AAR and native `.so` into the Minecraft process. Verify FCL supports this before integrating it into DiscordMobileBridge.
- Send updates only when activity changes, not every render tick.

## Official documentation
- https://discord.com/developers/docs/social-sdk/index.html
- https://discord.com/developers/docs/social-sdk/release_notes.html
- https://discord.com/developers/docs/social-sdk/classdiscordpp_1_1Client.html
- https://discord.com/developers/docs/social-sdk/classdiscordpp_1_1Activity.html
- https://discord.com/blog/mobile-platform-support-arrives-in-discord-social-sdk

## License
Bridge source: MIT. Discord Social SDK: separate proprietary dependency under Discord's terms. Not affiliated with or endorsed by Discord.
