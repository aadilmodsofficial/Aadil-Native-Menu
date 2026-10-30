
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# KittyMemory
KITTYMEMORY_PATH = KittyMemory
include $(CLEAR_VARS)

LOCAL_MODULE := Keystone
LOCAL_SRC_FILES := $(KITTYMEMORY_PATH)/Deps/Keystone/libs-android/$(TARGET_ARCH_ABI)/libkeystone.a
include $(PREBUILT_STATIC_LIBRARY)

# Aadil Mods Native Library
include $(CLEAR_VARS)
LOCAL_MODULE := AadilMods

LOCAL_CFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions

LOCAL_CPPFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++17
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions

# 16 KB page size support
LOCAL_LDFLAGS += -Wl,-z,max-page-size=16384

LOCAL_LDFLAGS += -Wl,--gc-sections -Wl,--strip-all
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2

LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Includes/

# Source files
LOCAL_SRC_FILES := Main.cpp \
    KittyMemory/KittyArm64.cpp \
    KittyMemory/KittyScanner.cpp \
    KittyMemory/KittyMemory.cpp \
    KittyMemory/KittyUtils.cpp \
    KittyMemory/MemoryPatch.cpp \
    KittyMemory/MemoryBackup.cpp \
    And64InlineHook/And64InlineHook.cpp

LOCAL_STATIC_LIBRARIES := Keystone

include $(BUILD_SHARED_LIBRARY)
