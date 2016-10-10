#
# Created on: 2016-2-16
#     Author: Wang Yang
#       Mail: admin@wysaid.org
#

MAKEFILE_PATH := $(call my-dir)

###############################

#include $(CLEAR_VARS)

#OPENCV_INSTALL_MODULES:=on
#OPENCV_LIB_TYPE:=STATIC

#CV_ANDROID=/Users/wysaid/android_develop/opencv-android-sdk3.1

#include $(CV_ANDROID)/sdk/native/jni/OpenCV.mk

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

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_calib3d
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_calib3d.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_features2d
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_features2d.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_flann
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_flann.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_imgcodecs
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_imgcodecs.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_ml
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_ml.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_photo
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_photo.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_shape
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_shape.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_stitching
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_stitching.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_superres
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_superres.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_video
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_video.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_videoio
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_videoio.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := opencv_videostab
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libopencv_videostab.a
# include $(PREBUILT_STATIC_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := IlmImf
# LOCAL_SRC_FILES := $(CVLIB_DIR)/libIlmImf.a
# include $(PREBUILT_STATIC_LIBRARY)

###############################


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

# opencv_imgcodecs
# opencv_calib3d \
# opencv_features2d \
# opencv_flann \
# opencv_imgcodecs \
# opencv_ml \
# opencv_photo \
# opencv_shape \
# opencv_stitching \
# opencv_superres \
# opencv_video \
# opencv_videoio \
# opencv_videostab \
# IlmImf

LOCAL_CPPFLAGS := -frtti -std=gnu++11
LOCAL_LDLIBS :=  -llog -ljnigraphics -lz -ldl -lm

LOCAL_CFLAGS    := -D_CGE_LOGS_ -DANDROID_NDK -DCGE_LOG_TAG=\"libCGE\" -D__STDC_CONSTANT_MACROS -O3 -ffast-math -funroll-loops

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS := $(LOCAL_CFLAGS) -march=armv7-a -mfpu=neon -mfloat-abi=softfp
LOCAL_ARM_NEON := true
endif

ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_CFLAGS := $(LOCAL_CFLAGS) -mfloat-abi=softfp
endif

LOCAL_SHARED_LIBRARIES := opencv CGE

LOCAL_ARM_MODE := arm

LOCAL_EXPORT_C_INCLUDES := $(TRACKER_ROOT)

include $(BUILD_SHARED_LIBRARY)



