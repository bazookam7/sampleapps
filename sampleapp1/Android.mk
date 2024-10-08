
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
LOCAL_USE_AAPT2 := true

# LOCAL_STATIC_JAVA_LIBRARIES := android-common

# LOCAL_STATIC_JAVA_LIBRARIES += android-support-v4

# LOCAL_STATIC_JAVA_LIBRARIES += android-support-v7-appcompat

LOCAL_STATIC_ANDROID_LIBRARIES := \
    androidx.core_core \
    androidx.media_media \
    androidx.legacy_legacy-support-core-utils \
    androidx.legacy_legacy-support-core-ui \
    androidx.fragment_fragment \
    androidx.appcompat_appcompat \
    androidx.palette_palette \
    androidx.recyclerview_recyclerview \
    androidx.legacy_legacy-support-v13 \
    colorpicker \
    libchips \
    libphotoviewer \
	

LOCAL_STATIC_JAVA_LIBRARIES := \
    androidx.annotation_annotation \
    android-common \
    android-common-framesequence \
	# android-support-v7-appcompat \
	# android-support-v4 \
	# android-support-v7-recyclerview\

# include $(LOCAL_PATH)/version.mk

# LOCAL_AAPT_FLAGS += --version-name "$(version_name_package)"
# LOCAL_AAPT_FLAGS += --version-code $(version_code_package)

# ifdef TARGET_BUILD_APPS
#     LOCAL_JNI_SHARED_LIBRARIES := libframesequence libgiftranscode
# else
#     LOCAL_REQUIRED_MODULES:= libframesequence libgiftranscode
# endif

# LOCAL_PROGUARD_ENABLED := obfuscation optimization

# LOCAL_PROGUARD_FLAG_FILES := proguard.flags
# ifeq (eng,$(TARGET_BUILD_VARIANT))
#     LOCAL_PROGUARD_FLAG_FILES += proguard-test.flags
# else
#     LOCAL_PROGUARD_FLAG_FILES += proguard-release.flags
# endif

LOCAL_PACKAGE_NAME := sampleapp1

LOCAL_CERTIFICATE := platform

LOCAL_SDK_VERSION := current
LOCAL_PRIVILEGED_MODULE := true
LOCAL_PRODUCT_MODULE := true

LOCAL_MODULE_PATH := $(TARGET_OUT_PRODUCT_APPS)

LOCAL_COMPATIBILITY_SUITE := general-tests

include $(BUILD_PACKAGE)

include $(call all-makefiles-under, $(LOCAL_PATH))