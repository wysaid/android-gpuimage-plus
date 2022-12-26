/*
 * cge.h
 *
 *  Created on: 2014-10-16
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGE_H_
#define _CGE_H_

//umbrella header

#ifndef IOS_SDK
#define IOS_SDK 1
#endif

#ifndef CGE_TEXTURE_PREMULTIPLIED
#define CGE_TEXTURE_PREMULTIPLIED 1
#endif

#ifndef _CGE_ONLY_FILTERS_
#define _CGE_ONLY_FILTERS_ 1
#endif

#ifdef __OBJC__

#import <UIKit/UIKit.h>

//! Project version number for cge.
FOUNDATION_EXPORT double cgeVersionNumber;

//! Project version string for cge.
FOUNDATION_EXPORT const unsigned char cgeVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <cge/PublicHeader.h>

#import <cge/cgeCommonDefine.h>
#import <cge/cgeGlobal.h>

#import <cge/cgeCVUtilTexture.h>
#import <cge/cgeCameraDevice.h>
#import <cge/cgeCameraFrameRecorder.h>
#import <cge/cgeDynamicImageViewHandler.h>
#import <cge/cgeFrameRecorder.h>
#import <cge/cgeFrameRenderer.h>
#import <cge/cgeImageViewHandler.h>
#import <cge/cgeProcessingContext.h>
#import <cge/cgeSharedGLContext.h>
#import <cge/cgeUtilFunctions.h>
#import <cge/cgeVideoCameraViewHandler.h>
#import <cge/cgeVideoFrameRecorder.h>
#import <cge/cgeVideoPlayer.h>
#import <cge/cgeVideoPlayerViewHandler.h>
#import <cge/cgeVideoWriter.h>

#ifdef __cplusplus

#import <cge/cgeVideoHandlerCV.h>
#import <cge/cgeImageHandlerIOS.h>

// pure cpp

#import <cge/cgeFilters.h>
#import <cge/cgeGLFunctions.h>
#import <cge/cgeImageFilter.h>
#import <cge/cgeImageHandler.h>
#import <cge/cgeMat.h>
#import <cge/cgeScene.h>
#import <cge/cgeShaderFunctions.h>
#import <cge/cgeStaticAssert.h>
#import <cge/cgeTextureUtils.h>
#import <cge/cgeThread.h>
#import <cge/cgeVec.h>
#import <cge/cgeAdvancedEffects.h>
#import <cge/cgeAdvancedEffectsCommon.h>
#import <cge/cgeAlienLookFilter.h>
#import <cge/cgeBeautifyFilter.h>
#import <cge/cgeBilateralBlurFilter.h>
#import <cge/cgeBlendFilter.h>
#import <cge/cgeBrightnessAdjust.h>
#import <cge/cgeColorBalanceAdjust.h>
#import <cge/cgeColorLevelAdjust.h>
#import <cge/cgeContrastAdjust.h>
#import <cge/cgeCrosshatchFilter.h>
#import <cge/cgeCurveAdjust.h>
#import <cge/cgeDataParsingEngine.h>
#import <cge/cgeDistortionFilter.h>
#import <cge/cgeDynamicFilters.h>
#import <cge/cgeDynamicWaveFilter.h>
#import <cge/cgeEdgeFilter.h>
#import <cge/cgeEmbossFilter.h>
#import <cge/cgeEnlargeEyeFilter.h>
#import <cge/cgeExposureAdjust.h>
#import <cge/cgeEyeBrightenFilter.h>
#import <cge/cgeEyeTintFilter.h>
#import <cge/cgeFilterBasic.h>
#import <cge/cgeHalftoneFilter.h>
#import <cge/cgeHazeFilter.h>
#import <cge/cgeHueAdjust.h>
#import <cge/cgeLerpblurFilter.h>
#import <cge/CGELiquifyFilter.h>
#import <cge/cgeLookupFilter.h>
#import <cge/cgeMarscaraFilter.h>
#import <cge/cgeMaxValueFilter.h>
#import <cge/cgeMidValueFilter.h>
#import <cge/cgeMinValueFilter.h>
#import <cge/cgeMonochromeAdjust.h>
#import <cge/cgeMosaicBlurFilter.h>
#import <cge/cgeMultipleEffects.h>
#import <cge/cgeMultipleEffectsCommon.h>
#import <cge/cgeMultipleEffectsDataset.h>
#import <cge/cgePolarPixellateFilter.h>
#import <cge/cgePolkaDotFilter.h>
#import <cge/cgeRandomBlurFilter.h>
#import <cge/cgeSaturationAdjust.h>
#import <cge/cgeShadowHighlightAdjust.h>
#import <cge/cgeSharpenBlurAdjust.h>
#import <cge/cgeSketchFilter.h>
#import <cge/cgeTiltshiftAdjust.h>
#import <cge/cgeVignetteAdjust.h>
#import <cge/cgeWhiteBalanceAdjust.h>


#endif

#endif

#endif
