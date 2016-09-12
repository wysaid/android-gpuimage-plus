#
# Created on: 2015-7-9
#     Author: Wang Yang
#       Mail: admin@wysaid.org
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := CGE

#*********************** CGE Library ****************************

CGE_ROOT=$(LOCAL_PATH)

CGE_SOURCE=$(CGE_ROOT)/src

CGE_INCLUDE=$(CGE_ROOT)/include

#### CGE Library headers ###########
LOCAL_EXPORT_C_INCLUDES := \
					$(CGE_ROOT)/interface \
					$(CGE_INCLUDE) \
					$(CGE_INCLUDE)/filters \


#### CGE Library native source  ###########

LOCAL_SRC_FILES :=  \
				cge/$(TARGET_ARCH_ABI)/libCGE.so


LOCAL_CPPFLAGS := -frtti -std=gnu++11

# 'CGE_USE_VIDEO_MODULE' determines if the project should compile with ffmpeg.

ifdef CGE_USE_VIDEO_MODULE

VIDEO_MODULE_DEFINE = -D_CGE_USE_FFMPEG_ 

endif

ifndef CGE_RELEASE_MODE

BUILD_MODE = -D_CGE_LOGS_

endif

LOCAL_CFLAGS    := ${VIDEO_MODULE_DEFINE} ${BUILD_MODE} -DANDROID_NDK -DCGE_LOG_TAG=\"libCGE\" -DCGE_TEXTURE_PREMULTIPLIED=1 -D__STDC_CONSTANT_MACROS -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -ffast-math -D_CGE_ONLY_FILTERS_

ifdef CGE_USE_FACE_MODULE

LOCAL_CFLAGS := $(LOCAL_CFLAGS) -D_CGE_USE_FACE_MODULE_

endif

ifndef CGE_USE_VIDEO_MODULE

#LOCAL_CFLAGS := $(LOCAL_CFLAGS) -D_CGE_ONLY_FILTERS_

include $(PREBUILT_SHARED_LIBRARY)

else 

LOCAL_SHARED_LIBRARIES := ffmpeg

include $(PREBUILT_SHARED_LIBRARY)

################################

# include $(CLEAR_VARS)
# LOCAL_MODULE := x264
# LOCAL_CFLAGS := -march=armv7-a -mfloat-abi=softfp -mfpu=neon -O3 -ffast-math -funroll-loops
# LOCAL_SRC_FILES := ffmpeg/libx264.142.so
# #LOCAL_EXPORT_C_INCLUDES := $(CGE_ROOT)/ffmpeg
# include $(PREBUILT_SHARED_LIBRARY)

###############################

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_CFLAGS := -mfloat-abi=softfp -mfpu=vfp -O3 -ffast-math -funroll-loops -fPIC
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS := $(LOCAL_CFLAGS) march=armv7-a -mfpu=neon
endif
LOCAL_SRC_FILES := ffmpeg/$(TARGET_ARCH_ABI)/libffmpeg.so
LOCAL_EXPORT_C_INCLUDES := $(CGE_ROOT)/ffmpeg

# LOCAL_SHARED_LIBRARIES := x264

include $(PREBUILT_SHARED_LIBRARY)

endif

###############################

ifdef CGE_USE_FACE_MODULE

include $(CLEAR_VARS)
include $(CGE_ROOT)/faceTracker/jni/Android.mk

endif
###############################

# Call user defined module
include $(CLEAR_VARS)
include $(CGE_ROOT)/source/source.mk