/*
 * cgeFaceTracker.h
 *
 *  Created on: 2016-1-16
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 *         ref: 参考了 ofxFaceTracker ( https://github.com/kylemcdonald/ofxFaceTracker )
 */

#ifndef _CGE_FACETRACKER_H_
#define _CGE_FACETRACKER_H_

#include "Tracker.h"
#include "cgeVec.h"
#include "cgeFaceCommon.h"

namespace CGE
{
    class CGEFaceTracker
    {
    public:
        CGEFaceTracker();
        ~CGEFaceTracker();
        
        static void setupTracker(const char* modelfname,const char* trifname,const char* confname); //使用外部文件初始化
        static void setupTracker(); //使用内部数据初始化
        static inline bool isTrackerSetup() { return _isTrackerOK; }
        
        /*
         faceImage: 待检测的人脸图像 (必须为单通道图像)
         framePerDetection: 每一次检测间隔多少帧 (-1 表示无间隔
         iteration: 执行的最大优化步骤
         clamp: 形状模型缩紧参数
         tolerance: 收敛优化
         checkStatus: 是否检测失败与否
         返回值: 成功true, 失败false
        */
        
        bool updateFace(cv::Mat& faceImage, int framePerDetection = -1, int iteration = 5, double clamp = 3.0, double tolerance = 0.01, bool checkStatus = false);
        
        //windowSize: 检测窗口大小， 取值 0 最小, 1 中等, 2 最大
        void setWindowSize(int windowSize);
        
        //最大使用图像尺寸 (当传入图像尺寸大于它时将进行裁剪, size <= 0 表示不裁剪)
        inline void setMaxImageSize(int size) { m_maxImageSize = size; }
        
        inline int getFaceImageWidth() const { return m_faceImageSize.width; }
        inline int getFaceImageHeight() const { return m_faceImageSize.height; }
        inline int getScaledFaceImageWidth() const { return m_scaledImageSize.width; }
        inline int getScaledFaceImageHeight() const { return m_scaledImageSize.height; }
        
        static const std::vector<int>& getFeatureIndices(CGEFaceFeature feature);
        
        inline const std::vector<Vec2f>& getPointList() const { return m_pointList[m_pointListIndex]; }
        inline const Vec4f& getFaceRect() const { return m_faceRect; } //脸部位置
        bool isMouthOpen() const; //嘴巴是否张开
        Vec2f getMouthPos() const; //嘴巴位置
        
        Vec2f getLeftEyePos() const; //左眼位置
        Vec2f getRightEyePos() const; //右眼位置
        Vec2f getUpDir() const; //头部向上方向 (以左右双眼形成的向量作为参考)
        Vec2f getRightDir() const; //头部水平方向 (以左右双眼形成的向量作为参考)
        
        Vec2f getCenterPos() const; //面部中心位置
        Vec2f getNoseCenterPos() const; //鼻子中心位置
        Vec2f getEyeCenterPos() const; //双眼正中位置
        Vec2f getJawBottomPos() const; //下巴最下方位置
        
        static void resetFrame() { _tracker.FrameReset(); }
        
        void drawMeshes(cv::Mat& dst, int thickness = 2, const cv::Scalar scalar = cv::Scalar(255, 255, 255, 255)) const;
        
        void drawFeature(cv::Mat& dst, CGEFaceFeature feature, bool cycle = true, int thickness = 2, const cv::Scalar scalar = cv::Scalar(255, 255, 255, 255)) const;
        
        inline static FACETRACKER::Tracker& getTracker() { return _tracker; }
        inline cv::Mat& getTriModel() { return _triModel; }
        inline cv::Mat& getConModel() { return _conModel; }
        inline bool hasFacePoints() const { return !m_pointList[m_pointListIndex].empty(); }
        
    protected:
        
        inline std::vector<Vec2f>& getPointListCache() { return m_pointList[1 - m_pointListIndex]; }
        inline void switchPointList() { m_pointListIndex = 1 - m_pointListIndex; }
        
        static std::vector<int> vecFaceIndices[CGE_FACE_TOTAL_FEATURE_NUM];
        
        static FACETRACKER::Tracker _tracker;
        static cv::Mat _triModel;
        static cv::Mat _conModel;
        static bool _isTrackerOK;
        
    private:
        
        cv::Mat m_cacheImage;
        
        std::vector<Vec2f> m_pointList[2];
        int m_pointListIndex;
        
        Vec4f m_faceRect;
        
        //settings
        std::vector<int> m_minWindowSize, m_midWindowSize, m_maxWindowSize;
        std::vector<int> *m_currentWindowSize, *m_tmpWindowSize;
        
        cv::Size m_faceImageSize;
        cv::Size m_scaledImageSize;

        int m_maxImageSize;
        float m_imageScaling;
        
        int m_failCount;
        int m_sucCount;
    };
}

#endif