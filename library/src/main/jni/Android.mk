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

CGE_SOURCE=$(CGE_ROOT)/cge

#### CGE Library headers ###########
LOCAL_C_INCLUDES := \
					$(CGE_ROOT)/interface \
					$(CGE_SOURCE)/common \
					$(CGE_SOURCE)/extends \
					$(CGE_SOURCE)/filters \


#### CGE Library native source  ###########

LOCAL_SRC_FILES :=  \
			$(CGE_SOURCE)/common/cgeCommonDefine.cpp \
			$(CGE_SOURCE)/common/cgeGLFunctions.cpp \
			$(CGE_SOURCE)/common/cgeImageFilter.cpp \
			$(CGE_SOURCE)/common/cgeImageHandler.cpp \
			$(CGE_SOURCE)/common/cgeShaderFunctions.cpp \
			$(CGE_SOURCE)/common/cgeGlobal.cpp \
			$(CGE_SOURCE)/common/cgeTextureUtils.cpp \
			$(CGE_SOURCE)/common/cgeGL3Stub.c \
			$(CGE_SOURCE)/common/cgeGL31Stub.c \
			\
			$(CGE_SOURCE)/filters/cgeAdvancedEffects.cpp \
			$(CGE_SOURCE)/filters/cgeAdvancedEffectsCommon.cpp \
			$(CGE_SOURCE)/filters/cgeBilateralBlurFilter.cpp \
			$(CGE_SOURCE)/filters/cgeMosaicBlurFilter.cpp \
			$(CGE_SOURCE)/filters/cgeBeautifyFilter.cpp \
			\
			$(CGE_SOURCE)/filters/cgeBrightnessAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeColorLevelAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeContrastAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeCurveAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeExposureAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeFilterBasic.cpp \
			$(CGE_SOURCE)/filters/cgeHueAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeMonochromeAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeSaturationAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeSelectiveColorAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeShadowHighlightAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeSharpenBlurAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeTiltshiftAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeVignetteAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeWhiteBalanceAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeColorBalanceAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeLookupFilter.cpp \
			\
			$(CGE_SOURCE)/filters/cgeBlendFilter.cpp \
			\
			$(CGE_SOURCE)/filters/cgeDataParsingEngine.cpp \
			$(CGE_SOURCE)/filters/cgeMultipleEffects.cpp \
			$(CGE_SOURCE)/filters/cgeMultipleEffectsCommon.cpp \
			\
			$(CGE_SOURCE)/filters/cgeHazeFilter.cpp \
			$(CGE_SOURCE)/filters/cgePolarPixellateFilter.cpp \
			$(CGE_SOURCE)/filters/cgePolkaDotFilter.cpp \
			$(CGE_SOURCE)/filters/cgeHalftoneFilter.cpp \
			$(CGE_SOURCE)/filters/cgeEdgeFilter.cpp \
			$(CGE_SOURCE)/filters/cgeEmbossFilter.cpp \
			\
			$(CGE_SOURCE)/filters/cgeWaveformFilter.cpp \
			\
			$(CGE_SOURCE)/filters/cgeCrosshatchFilter.cpp \
			$(CGE_SOURCE)/filters/cgeLiquifyFilter.cpp \
			$(CGE_SOURCE)/filters/cgeRandomBlurFilter.cpp \
			$(CGE_SOURCE)/filters/cgeMinValueFilter.cpp \
			$(CGE_SOURCE)/filters/cgeMaxValueFilter.cpp \
			$(CGE_SOURCE)/filters/cgeSketchFilter.cpp \
			$(CGE_SOURCE)/filters/cgeLerpblurFilter.cpp \
			\
			$(CGE_SOURCE)/filters/cgeDynamicFilters.cpp \
			$(CGE_SOURCE)/filters/cgeDynamicWaveFilter.cpp \
			$(CGE_SOURCE)/filters/cgeMotionFlowFilter.cpp \
			$(CGE_SOURCE)/filters/cgeColorMappingFilter.cpp \
			$(CGE_SOURCE)/extends/cgeThread.cpp \
			\
			$(CGE_ROOT)/interface/cgeNativeLibrary.cpp \
			$(CGE_ROOT)/interface/cgeFFmpegNativeLibrary.cpp \
			$(CGE_ROOT)/interface/cgeSharedGLContext.cpp \
			$(CGE_ROOT)/interface/cgeFrameRenderer.cpp \
			$(CGE_ROOT)/interface/cgeFrameRendererWrapper.cpp \
			$(CGE_ROOT)/interface/cgeFrameRecorder.cpp \
			$(CGE_ROOT)/interface/cgeFrameRecorderWrapper.cpp \
			$(CGE_ROOT)/interface/cgeVideoEncoder.cpp \
			$(CGE_ROOT)/interface/cgeUtilFunctions.cpp \
			$(CGE_ROOT)/interface/cgeVideoDecoder.cpp \
			$(CGE_ROOT)/interface/cgeVideoPlayer.cpp \
			$(CGE_ROOT)/interface/cgeImageHandlerAndroid.cpp \
			$(CGE_ROOT)/interface/cgeImageHandlerWrapper.cpp \
			$(CGE_ROOT)/interface/cgeDeformFilterWrapper.cpp \


LOCAL_CPPFLAGS := -frtti -std=c++14
LOCAL_LDLIBS :=  -llog -lEGL -lGLESv2 -ljnigraphics

ifdef CGE_ENABLE_16KB_PAGE_SIZE
LOCAL_LDFLAGS += "-Wl,-z,max-page-size=16384"
endif

# 'CGE_USE_VIDEO_MODULE' determines if the project should compile with ffmpeg.

ifdef CGE_USE_VIDEO_MODULE

VIDEO_MODULE_DEFINE = -DCGE_USE_FFMPEG 

endif

ifndef CGE_RELEASE_MODE
BUILD_MODE = -D_CGE_LOGS_
ifdef CGE_DEBUG_MODE
BUILD_MODE += -DDEBUG
endif
endif

LOCAL_CFLAGS    := ${VIDEO_MODULE_DEFINE} ${BUILD_MODE} -DANDROID_NDK -DCGE_LOG_TAG=\"libCGE\" -DCGE_TEXTURE_PREMULTIPLIED=1 -D__STDC_CONSTANT_MACROS -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -ffast-math -D_CGE_ONLY_FILTERS_

ifdef CGE_USE_FACE_MODULE

LOCAL_CFLAGS := $(LOCAL_CFLAGS) -D_CGE_USE_FACE_MODULE_

endif

ifndef CGE_USE_VIDEO_MODULE

#LOCAL_CFLAGS := $(LOCAL_CFLAGS) -D_CGE_ONLY_FILTERS_

include $(BUILD_SHARED_LIBRARY)

else 

LOCAL_SHARED_LIBRARIES := ffmpeg

include $(BUILD_SHARED_LIBRARY)

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
LOCAL_CFLAGS := -mfloat-abi=softfp -mfpu=vfp -Os -ffast-math -fPIC
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS := $(LOCAL_CFLAGS) march=armv7-a -mfpu=neon
endif
ifdef CGE_ENABLE_16KB_PAGE_SIZE
LOCAL_SRC_FILES := ffmpeg-16kb/libs/$(TARGET_ARCH_ABI)/libffmpeg.so
LOCAL_EXPORT_C_INCLUDES := $(CGE_ROOT)/ffmpeg-16kb/include
else
LOCAL_SRC_FILES := ffmpeg/libs/$(TARGET_ARCH_ABI)/libffmpeg.so
LOCAL_EXPORT_C_INCLUDES := $(CGE_ROOT)/ffmpeg/include
endif

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
include $(CGE_ROOT)/custom/source.mk