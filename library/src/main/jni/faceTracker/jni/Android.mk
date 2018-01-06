#
# Created on: 2016-2-16
#     Author: Wang Yang
#       Mail: admin@wysaid.org
#

MAKEFILE_PATH := $(call my-dir)

##############################

OPENCV_ROOT=$(MAKEFILE_PATH)/../../opencv

CVLIB_DIR=$(OPENCV_ROOT)/$(TARGET_ARCH_ABI)

include $(CLEAR_VARS)
LOCAL_MODULE := opencv_core
LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_core.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := opencv_imgproc
LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_imgproc.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := opencv_objdetect
LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_objdetect.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tbb
LOCAL_SRC_FILES := $(CVLIB_DIR)/libtbb.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE    := FaceTracker

#*********************** Tracker Library ****************************

TRACKER_ROOT=$(MAKEFILE_PATH)/..
TRACKER_SOURCE=$(TRACKER_ROOT)/FaceTracker
TRACKER_INCLUDE=$(TRACKER_ROOT)/FaceTracker

LOCAL_C_INCLUDES += \
					$(TRACKER_ROOT) \
					$(TRACKER_ROOT)/../include \
					$(TRACKER_INCLUDE) \
					$(OPENCV_ROOT) \
					$(OPENCV_ROOT)/opencv2 \


LOCAL_SRC_FILES :=  \
			$(TRACKER_SOURCE)/CLM.cpp \
			$(TRACKER_SOURCE)/FDet.cpp \
			$(TRACKER_SOURCE)/PAW.cpp \
			$(TRACKER_SOURCE)/Patch.cpp \
			$(TRACKER_SOURCE)/FCheck.cpp \
			$(TRACKER_SOURCE)/IO.cpp \
			$(TRACKER_SOURCE)/PDM.cpp \
			$(TRACKER_SOURCE)/Tracker.cpp \
			\
			$(TRACKER_ROOT)/cgeFaceTracker.cpp \
			$(TRACKER_ROOT)/cgeFaceTrackerWrapper.cpp \
			

LOCAL_STATIC_LIBRARIES := \
			opencv_objdetect \
			opencv_imgproc \
			opencv_core \
			tbb 

LOCAL_CPPFLAGS := -frtti -std=gnu++11
LOCAL_LDLIBS :=  -llog -ljnigraphics -lz -ldl -lm -latomic

LOCAL_CFLAGS    := -D_CGE_LOGS_ -DANDROID_NDK -DCGE_LOG_TAG=\"libCGE\" -D__STDC_CONSTANT_MACROS -O3 -ffast-math -funroll-loops

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS := $(LOCAL_CFLAGS) -march=armv7-a -mfpu=neon -mfloat-abi=softfp
LOCAL_ARM_NEON := true
endif

ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_CFLAGS := $(LOCAL_CFLAGS) -mfloat-abi=softfp
endif

LOCAL_SHARED_LIBRARIES := CGE

LOCAL_ARM_MODE := arm

LOCAL_EXPORT_C_INCLUDES := $(TRACKER_ROOT)

include $(BUILD_SHARED_LIBRARY)



