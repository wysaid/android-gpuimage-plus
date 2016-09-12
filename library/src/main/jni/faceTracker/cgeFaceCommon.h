//
//  cgeFaceCommon.h
//  cgeFaceTracker
//
//  Created by wysaid on 16/1/17.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#ifndef cgeFaceCommon_h
#define cgeFaceCommon_h

#ifdef __cplusplus
extern "C"
{
#endif
    
#define CGE_FACE_EFFECT_VERSION 20160126

typedef enum CGEFaceFeature {
    CGE_FACE_LEFT_EYE_TOP,
    CGE_FACE_RIGHT_EYE_TOP,
    CGE_FACE_LEFT_EYEBROW,
    CGE_FACE_RIGHT_EYEBROW,
    CGE_FACE_LEFT_EYE,
    CGE_FACE_RIGHT_EYE,
    CGE_FACE_LEFT_JAW,
    CGE_FACE_RIGHT_JAW,
    CGE_FACE_JAW,
    CGE_FACE_OUTER_MOUTH,
    CGE_FACE_INNER_MOUTH,
    CGE_FACE_NOSE_BRIDGE,
    CGE_FACE_NOSE_BASE,
    CGE_FACE_OUTLINE,
    CGE_FACE_ALL_FEATURES,
    CGE_FACE_TOTAL_FEATURE_NUM
}CGEFaceFeature;

typedef enum CGETrackingEffectMode
{
    CGE_TE_INVALID, //非法特效标记
    
    // 检测到张嘴动作时播放视频
    CGE_TE_OPENMOUTH_PLAYVIDEO_ADD,
    
    CGE_TE_OPENMOUTH_PLAYVIDEO_MASK,
    
    // 头顶跟踪显示一张图片, 同时检测到张嘴动作时播放视频
    CGE_TE_OPENMOUTH_PLAYVIDEO_ADD_x_IMAGE_ABOVE_HEAD,
    
    // 头顶跟踪显示一个视频(视频播放完毕之后停留在最后一帧, 同时检测到张嘴动作时播放另一个视频
    CGE_TE_OPENMOUTH_PLAYVIDEO_ADD_x_VIDEO_ABOVE_HEAD,

    CGE_TE_OPENMOUTH_PLAYVIDEO_MASK_x_IMAGE_ABOVE_HEAD,
    
    CGE_TE_OPENMOUTH_PLAYVIDEO_MASK_x_VIDEO_ABOVE_HEAD,
    
    CGE_TE_OPENMOUTH_TRACKINGPLAYVIDEO,
    
    CGE_TE_PLAY2MIXEDVIDEO,
    
    CGE_TRACKING_EFFECT_TOTAL_NUM
}CGETrackingEffectMode;

typedef enum CGETrackingEffectPlayMode
{
    CGE_TE_PLAY_REPEAT, //default
    CGE_TE_PLAY_ONCE_STILL,
    CGE_TE_PLAY_ONCE_DISAPPEAR,
    CGE_TE_PLAY_LOOP
    
}CGETrackingEffectPlayMode;
    
typedef enum CGETrackingEffectFireEvent
{
    CGE_TE_FIRE_EVENT_NONE, //default
    CGE_TE_FIRE_EVENT_ANY, 
    CGE_TE_FIRE_EVENT_FAE_APPEAR,
    CGE_TE_FIRE_EVENT_MOUTH_OPEN,
    CGE_TE_FIRE_EVENT_EYEBROW_RAISE,
    
    CGE_TE_FIRE_EVENT_NUM
}CGETrackingEffectFireEvent;
    
#ifdef __cplusplus
}
#endif

#endif /* cgeFaceCommon_h */
