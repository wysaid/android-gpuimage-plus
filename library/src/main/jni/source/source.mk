#
# Created on: 2015-7-9
#     Author: Wang Yang
#       Mail: admin@wysaid.org
#

CGEEXT_ROOT := $(call my-dir)
CGE_ROOT := $(CGEEXT_ROOT)/..
include $(CLEAR_VARS)

LOCAL_MODULE    := CGEExt

#*********************** CGE Library ****************************

#### CGE Library headers ###########
LOCAL_C_INCLUDES := \
			\
			$(CGEEXT_ROOT) \
			$(CGEEXT_ROOT)/../include \
			$(CGEEXT_ROOT)/../include/filters \
			$(CGEEXT_ROOT)/../interface \


#### CGE Library native source  ###########

LOCAL_SRC_FILES :=  \
			$(CGEEXT_ROOT)/cgeVideoUtils.cpp \
			$(CGEEXT_ROOT)/cgeCustomFilters.cpp \
			$(CGEEXT_ROOT)/cgeCustomFilterWrapper.cpp \
			$(CGEEXT_ROOT)/customHelper.cpp \
			$(CGEEXT_ROOT)/customFilter_0.cpp \
			$(CGEEXT_ROOT)/customFilter_N.cpp \


LOCAL_CPPFLAGS := -frtti -std=gnu++11
LOCAL_LDLIBS :=  -llog -lEGL -lGLESv2 -ljnigraphics -latomic

LOCAL_CFLAGS    := -DANDROID_NDK -DCGE_LOG_TAG=\"cgeExt\" -DCGE_TEXTURE_PREMULTIPLIED=1 -D__STDC_CONSTANT_MACROS -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -ffast-math -D_CGE_ONLY_FILTERS_ -D_CGE_LOGS_

ifdef CGE_USE_VIDEO_MODULE

LOCAL_CFLAGS := $(LOCAL_CFLAGS) -D_CGE_USE_FFMPEG_ 

LOCAL_SHARED_LIBRARIES := CGE ffmpeg

else 

LOCAL_SHARED_LIBRARIES := CGE

endif

include $(BUILD_SHARED_LIBRARY)