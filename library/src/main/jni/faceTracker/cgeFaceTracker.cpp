/*
 * cgeFaceTracker.cpp
 *
 *  Created on: 2016-1-16
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeFaceTracker.h"
#include "cgeCommonDefine.h"

#define FACETRACKER_DEFAULT_IMAGE_SIZE 320

namespace CGE
{
    FACETRACKER::Tracker CGEFaceTracker::_tracker;
    cv::Mat CGEFaceTracker::_triModel;
    cv::Mat CGEFaceTracker::_conModel;
    bool CGEFaceTracker::_isTrackerOK = false;
    
    CGEFaceTracker::CGEFaceTracker() : m_currentWindowSize(&m_midWindowSize), m_faceImageSize(0, 0), m_scaledImageSize(0, 0), m_maxImageSize(FACETRACKER_DEFAULT_IMAGE_SIZE), m_imageScaling(1.0f),  m_pointListIndex(0), m_failCount(0), m_tmpWindowSize(nullptr), m_sucCount(0)
    {
        int maxWindowSizes[] = {
            19, 17, 15, 13, 11, 9, 7
        };
        
        int midWindowSizes[] = {
            11, 9, 7
        };
        
        int minWindowSizes[] = {
            7
        };
        
        for(int i : maxWindowSizes)
        {
            m_maxWindowSize.push_back(i);
        }
        
        for(int i : midWindowSizes)
        {
            m_midWindowSize.push_back(i);
        }
        
        for(int i : minWindowSizes)
        {
            m_minWindowSize.push_back(i);
        }
    }
    
    CGEFaceTracker::~CGEFaceTracker()
    {
        for(auto& v : vecFaceIndices)
        {
            v.clear();
        }
    }
    
    void CGEFaceTracker::setupTracker(const char *modelfname, const char *trifname, const char *confname)
    {
        if(_isTrackerOK)
        {
            _tracker.FrameReset();
        }
        else
        {
            _tracker.Load(modelfname);
            _triModel = FACETRACKER::IO::LoadTri(trifname);
            _conModel = FACETRACKER::IO::LoadCon(confname);
            _isTrackerOK = true;
        }
    }
    
    void CGEFaceTracker::setupTracker()
    {
        if(_isTrackerOK)
        {
            _tracker.FrameReset();
        }
        else
        {
            const char* ftData =
#include "model/face2.tracker"
            ;
            const char* triData =
#include "model/face.tri"
            ;
            const char* conData =
#include "model/face.con"
            ;
            _tracker.LoadFromData(ftData);
            _triModel = FACETRACKER::IO::LoadTriByData(triData);
            _conModel = FACETRACKER::IO::LoadConByData(conData);
            _isTrackerOK = true;
        }
    }
    
    bool CGEFaceTracker::updateFace(cv::Mat& faceImage, int framePerDetection, int iteration, double clamp, double tolerance, bool checkStatus)
    {
        
        if(m_faceImageSize.width != faceImage.cols || m_faceImageSize.height != faceImage.rows)
        {
            m_faceImageSize.width = faceImage.cols;
            m_faceImageSize.height = faceImage.rows;
            m_imageScaling = m_maxImageSize > 0 ? (m_maxImageSize / (float)MIN(m_faceImageSize.width, m_faceImageSize.height)) : 1.0f;
            
            if(m_imageScaling < 1)
            {
                m_scaledImageSize.width = m_faceImageSize.width * m_imageScaling;
                m_scaledImageSize.height = m_faceImageSize.height * m_imageScaling;
            }
            else
            {
                m_scaledImageSize = m_faceImageSize;
            }
        }
        

        cv::resize(faceImage, m_cacheImage, m_scaledImageSize);

        if(m_failCount > 2 && m_currentWindowSize != &m_maxWindowSize)
        {
            if(m_tmpWindowSize == nullptr)
                m_tmpWindowSize = m_currentWindowSize;
            
            CGE_LOG_INFO("Jump to max precision\n");
            m_currentWindowSize = &m_maxWindowSize;
            
            m_failCount = 0;
        }
        
        if(m_sucCount > 60) // force update
        {
            checkStatus = true;
            iteration = 10;
            tolerance = 0.01;
        }
        
        if(_tracker.Track(m_cacheImage, *m_currentWindowSize, framePerDetection, iteration, clamp, tolerance, checkStatus) != 0)
        {
            _tracker.FrameReset();
            
            if(m_currentWindowSize != &m_maxWindowSize)
            {
                ++m_failCount;
            }
            
            m_sucCount = 0;
            
            return false;
        }
        
        ++m_sucCount;
        
        if(m_tmpWindowSize != nullptr)
        {
            m_currentWindowSize = m_tmpWindowSize;
            m_tmpWindowSize = nullptr;
            CGE_LOG_INFO("face detected\n");
        }
        
        m_failCount = 0;

        m_faceRect[0] = _tracker._rect.x / (float)m_scaledImageSize.width;
        m_faceRect[1] = _tracker._rect.y / (float)m_scaledImageSize.height;
        m_faceRect[2] = _tracker._rect.width / (float)m_scaledImageSize.width;
        m_faceRect[3] = _tracker._rect.height / (float)m_scaledImageSize.height;
        
        auto& pointList = getPointListCache();
        int pointNum = _tracker._shape.rows / 2;
        
        pointList.resize(pointNum);
        
        if(m_faceImageSize == m_scaledImageSize)
        {
            for(int i = 0; i < pointNum; ++i)
            {
                pointList[i] = Vec2f(_tracker._shape.at<double>(i, 0), _tracker._shape.at<double>(i + pointNum, 0));
            }
        }
        else
        {
            for(int i = 0; i < pointNum; ++i)
            {
                pointList[i] = Vec2f(_tracker._shape.at<double>(i, 0) / m_imageScaling, _tracker._shape.at<double>(i + pointNum, 0) / m_imageScaling);
            }
        }
        
        switchPointList();
        return true;
    }
    
    
    void CGEFaceTracker::setWindowSize(int windowSize)
    {
        switch (windowSize)
        {
            default:
            case 0:
                m_currentWindowSize = &m_minWindowSize;
                break;
            case 1:
                m_currentWindowSize = &m_midWindowSize;
                break;
            case 2:
                m_currentWindowSize = &m_maxWindowSize;
                break;
        }
    }
    
    void CGEFaceTracker::drawMeshes(cv::Mat &dst, int thickness, cv::Scalar scalar) const
    {
        auto& pointList = getPointList();
        Vec2f imgSize(m_faceImageSize.width, m_faceImageSize.height);
        
        for(int i = 0; i < _triModel.rows; i++)
        {
            const Vec2f& v1 = pointList[_triModel.at<int>(i,0)];
            const Vec2f& v2 = pointList[_triModel.at<int>(i,1)];
            const Vec2f& v3 = pointList[_triModel.at<int>(i,2)];
            const cv::Point p1(v1[0], v1[1]), p2(v2[0], v2[1]), p3(v3[0], v3[1]);
            
            cv::line(dst, p1, p2, scalar, thickness);
            cv::line(dst, p1, p3, scalar, thickness);
            cv::line(dst, p2, p3, scalar, thickness);
        }
    }
    
    void CGEFaceTracker::drawFeature(cv::Mat& dst, CGEFaceFeature feature, bool cycle, int thickness, cv::Scalar scalar) const
    {
        auto& indices = getFeatureIndices(feature);
        auto& pointList = getPointList();
        const int sz = (int)indices.size();
        
        for(int i = 1; i < sz; ++i)
        {
            const Vec2f& v1 = pointList[indices[i - 1]];
            const Vec2f& v2 = pointList[indices[i]];
            const cv::Point p1(v1[0], v1[1]), p2(v2[0], v2[1]);
            
            cv::line(dst, p1, p2, scalar, thickness);
        }
        
        if(cycle)
        {
            const Vec2f& v1 = pointList[indices[0]];
            const Vec2f& v2 = pointList[indices[sz - 1]];
            const cv::Point p1(v1[0], v1[1]), p2(v2[0], v2[1]);
            
            cv::line(dst, p1, p2, scalar, thickness);
        }
    }
    
    bool CGEFaceTracker::isMouthOpen() const
    {
        auto& indices = getFeatureIndices(CGE_FACE_INNER_MOUTH);
        auto& pointList = getPointList();

        const Vec2f& v1 = pointList[indices[2]];
        const Vec2f& v2 = pointList[indices[6]];
        return (v1 - v2).length() > 20.0f;
    }
    
    Vec2f CGEFaceTracker::getMouthPos() const
    {
        auto& indices = getFeatureIndices(CGE_FACE_INNER_MOUTH);
        auto& pointList = getPointList();
        
        const Vec2f& v1 = pointList[indices[2]];
        const Vec2f& v2 = pointList[indices[6]];
        return (v1 + v2) / 2.0f;
    }
    
    Vec2f CGEFaceTracker::getLeftEyePos() const
    {
        auto& indices = getFeatureIndices(CGE_FACE_LEFT_EYE_TOP);
        auto& pointList = getPointList();
        const Vec2f& v1 = pointList[indices[0]];
        const Vec2f& v2 = pointList[indices[3]];
        return (v1 + v2) / 2.0f;
    }
    
    Vec2f CGEFaceTracker::getRightEyePos() const
    {
        auto& indices = getFeatureIndices(CGE_FACE_RIGHT_EYE_TOP);
        auto& pointList = getPointList();
        const Vec2f& v1 = pointList[indices[0]];
        const Vec2f& v2 = pointList[indices[3]];
        return (v1 + v2) / 2.0f;
    }
    
    Vec2f CGEFaceTracker::getUpDir() const
    {
        const auto& v = getLeftEyePos() - getRightEyePos();
        return Vec2f(v[1], -v[0]);
    }
    
    Vec2f CGEFaceTracker::getRightDir() const
    {
        return getRightEyePos() - getLeftEyePos();
    }
    
    Vec2f CGEFaceTracker::getCenterPos() const
    {
        auto& pointList = getPointList();
        return pointList[30];
    }
    
    Vec2f CGEFaceTracker::getNoseCenterPos() const
    {
        auto& pointList = getPointList();
        return pointList[29];
    }
    
    Vec2f CGEFaceTracker::getEyeCenterPos() const
    {
        auto& pointList = getPointList();
        return pointList[27];
    }

    Vec2f CGEFaceTracker::getJawBottomPos() const
    {
        auto& pointList = getPointList();
        return pointList[8];
    }
    
#define CONSECUTIVE(a, b) \
do {\
int n = b - a;\
indices.resize(n);\
for(int i = 0; i != n; ++i)\
    indices[i] = a + i;\
}while(0)
    
    std::vector<int> CGEFaceTracker::vecFaceIndices[CGE_FACE_TOTAL_FEATURE_NUM];
    
    const std::vector<int>& CGEFaceTracker::getFeatureIndices(CGEFaceFeature feature)
    {
        assert(feature >= 0 && feature < CGE_FACE_TOTAL_FEATURE_NUM);
        
        std::vector<int>& indices = vecFaceIndices[feature];
        
        if(!indices.empty())
            return indices;
        
        switch(feature) {
            case CGE_FACE_LEFT_EYE_TOP: CONSECUTIVE(36, 40); break;
            case CGE_FACE_RIGHT_EYE_TOP: CONSECUTIVE(42, 46); break;
            case CGE_FACE_LEFT_JAW: CONSECUTIVE(0, 9); break;
            case CGE_FACE_RIGHT_JAW: CONSECUTIVE(8, 17); break;
            case CGE_FACE_JAW: CONSECUTIVE(0, 17); break;
            case CGE_FACE_LEFT_EYEBROW: CONSECUTIVE(17, 22); break;
            case CGE_FACE_RIGHT_EYEBROW: CONSECUTIVE(22, 27); break;
            case CGE_FACE_LEFT_EYE: CONSECUTIVE(36, 42); break;
            case CGE_FACE_RIGHT_EYE: CONSECUTIVE(42, 48); break;
            case CGE_FACE_OUTER_MOUTH: CONSECUTIVE(48, 60); break;
            case CGE_FACE_INNER_MOUTH:
            {
                static int innerMouth[] = {48,60,61,62,54,63,64,65};
                indices.assign(innerMouth, innerMouth + 8);
            }
                break;
            case CGE_FACE_NOSE_BRIDGE: CONSECUTIVE(27, 31); break;
            case CGE_FACE_NOSE_BASE: CONSECUTIVE(31, 36); break;
            case CGE_FACE_OUTLINE:
            {
                static int faceOutline[] = {17,18,19,20,21,22,23,24,25,26, 16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
                indices.assign(faceOutline, faceOutline + 27);
            }
                break;
            case CGE_FACE_ALL_FEATURES: CONSECUTIVE(0, 66); break;
            default:;
        }
        
        return indices;
    }
    
}






