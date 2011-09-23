
LOCAL_PATH:= $(call my-dir)
OPENSSL_INCLUDE_DIR :=/opt/android-ndk/external/openssl/include

CFLAGS := -Wpointer-arith -Wwrite-strings -Wunused -Winline \
 -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wno-long-long \
 -Wfloat-equal -Wno-multichar -Wsign-compare -Wno-format-nonliteral \
 -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement \
 -Wno-system-headers -DHAVE_CONFIG_H

include $(CLEAR_VARS)
include $(LOCAL_PATH)/curl/lib/Makefile.inc


LOCAL_SRC_FILES := $(addprefix curl/lib/,$(CSOURCES))
LOCAL_CFLAGS += $(CFLAGS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/curl/include/ $(LOCAL_PATH)/curl/lib $(OPENSSL_INCLUDE_DIR)

LOCAL_COPY_HEADERS_TO := libcurl
LOCAL_COPY_HEADERS := $(addprefix curl/include/curl/,$(HHEADERS))

LOCAL_MODULE:= libcurl

include $(BUILD_STATIC_LIBRARY)

# Build shared library now
# curltest

include $(CLEAR_VARS)

LOCAL_MODULE := curltest
LOCAL_SRC_FILES := curltest.c
LOCAL_STATIC_LIBRARIES := libcurl
LOCAL_C_INCLUDES += $(LOCAL_PATH)/curl/include $(LOCAL_PATH)/curl/lib
include $(BUILD_SHARED_LIBRARY) 

