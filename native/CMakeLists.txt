cmake_minimum_required(VERSION 3.22.1)
project(discord_social_bridge LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DISCORD_SDK_ROOT "" CACHE PATH "Extracted Discord Social SDK root")
set(DISCORD_SDK_LIBRARY "" CACHE FILEPATH "Exact path to libdiscord_partner_sdk.so")

add_library(discord_social_bridge SHARED)

if (DISCORD_SDK_ROOT AND EXISTS "${DISCORD_SDK_ROOT}/include/discordpp.h")
    if (NOT DISCORD_SDK_LIBRARY)
        find_file(DISCORD_SDK_LIBRARY
            NAMES libdiscord_partner_sdk.so
            PATHS
                "${DISCORD_SDK_ROOT}/lib/release/${ANDROID_ABI}"
                "${DISCORD_SDK_ROOT}/bin/release/${ANDROID_ABI}"
                "${DISCORD_SDK_ROOT}/lib/${ANDROID_ABI}"
                "${DISCORD_SDK_ROOT}/bin/${ANDROID_ABI}"
                "${DISCORD_SDK_ROOT}/lib/release"
                "${DISCORD_SDK_ROOT}/bin/release"
            NO_DEFAULT_PATH)
    endif()
    if (NOT DISCORD_SDK_LIBRARY OR NOT EXISTS "${DISCORD_SDK_LIBRARY}")
        message(FATAL_ERROR "SDK header found but Android libdiscord_partner_sdk.so was not found. Set -DDISCORD_SDK_LIBRARY=/absolute/path/to/library.")
    endif()

    add_library(discord_partner_sdk SHARED IMPORTED)
    set_target_properties(discord_partner_sdk PROPERTIES IMPORTED_LOCATION "${DISCORD_SDK_LIBRARY}")
    target_sources(discord_social_bridge PRIVATE discord_social_bridge.cpp)
    target_include_directories(discord_social_bridge PRIVATE "${DISCORD_SDK_ROOT}/include")
    target_link_libraries(discord_social_bridge PRIVATE discord_partner_sdk android log)
    message(STATUS "Using official Discord Social SDK: ${DISCORD_SDK_LIBRARY}")
else()
    target_sources(discord_social_bridge PRIVATE discord_social_bridge_stub.cpp)
    target_link_libraries(discord_social_bridge PRIVATE android log)
    message(WARNING "Discord Social SDK absent: building a non-functional stub AAR.")
endif()
