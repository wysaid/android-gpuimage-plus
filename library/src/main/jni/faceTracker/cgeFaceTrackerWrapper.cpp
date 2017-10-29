/*
* cgeFaceTrackerWrapper.cpp
*
*  Created on: 2016-2-17
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeFaceTrackerWrapper.h"
#include "cgeCommonDefine.h"

using namespace CGE;

extern "C"
{

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeSetupTracker(JNIEnv *env, jclass, jstring model, jstring tri, jstring con)
	{
		if(model == nullptr || tri == nullptr || con == nullptr)
		{
			CGEFaceTracker::setupTracker();
		}
		else
		{
			const char* modelStr = env->GetStringUTFChars(model, 0);
			const char* triStr = env->GetStringUTFChars(tri, 0);
			const char* conStr = env->GetStringUTFChars(con, 0);

			CGEFaceTracker::setupTracker(modelStr, triStr, conStr);

			env->ReleaseStringUTFChars(model, modelStr);
			env->ReleaseStringUTFChars(tri, triStr);
			env->ReleaseStringUTFChars(con, conStr);
		}

		CGE_LOG_INFO("JNI Face Tracker is OK!");
	}

	JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeCreateFaceTracker(JNIEnv *env, jobject)
	{
		CGEFaceTrackerWrapper* tracker = new CGEFaceTrackerWrapper();
		return (jlong)tracker;
	}

	JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeRelease(JNIEnv *env, jobject, jlong addr)
	{
		CGEFaceTrackerWrapper* tracker = (CGEFaceTrackerWrapper*)addr;
		delete tracker;
		CGE_LOG_INFO("tracker release...");
	}

	JNIEXPORT jfloatArray JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeDetectFaceWithSimpleResult(JNIEnv *env, jobject, jlong addr, jobject bmp, jboolean drawFeature)
	{
		CGEFaceTrackerWrapper* tracker = (CGEFaceTrackerWrapper*)addr;

		AndroidBitmapInfo info;
		int ret;
		char* row;

		if ((ret = AndroidBitmap_getInfo(env, bmp, &info)) < 0)
		{
			CGE_LOG_ERROR("AndroidBitmap_getInfo() failed ! error=%d", ret);
			return nullptr;
		}

		if (info.format != ANDROID_BITMAP_FORMAT_A_8 && info.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
		{
			CGE_LOG_ERROR("Bitmap format is not ALPHA_8 !");
			return nullptr;
		}

		CGE_LOG_INFO("color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d", info.width, info.height, info.stride, info.format, info.flags);

		ret = AndroidBitmap_lockPixels(env, bmp, (void**) &row);

		if (ret < 0)
		{
			CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
			return nullptr;
		}

		bool suc = tracker->trackImage(row, info.width, info.height, info.stride, (info.format == ANDROID_BITMAP_FORMAT_A_8 ? 1 : 4), drawFeature);
		
		AndroidBitmap_unlockPixels(env, bmp);

		if(suc)
		{
			jfloatArray arr = env->NewFloatArray(10);

			if(arr != nullptr)
			{
				jfloat data[10];
				auto* t = tracker->getTracker();
				const auto& leftEye = t->getLeftEyePos();
				const auto& rightEye = t->getRightEyePos();
				const auto& nose = t->getNoseCenterPos();
				const auto& mouth = t->getMouthPos();
				const auto& jawBottom = t->getJawBottomPos();

				data[0] = leftEye[0];
				data[1] = leftEye[1];
				data[2] = rightEye[0];
				data[3] = rightEye[1];
				data[4] = nose[0];
				data[5] = nose[1];
				data[6] = mouth[0];
				data[7] = mouth[1];
				data[8] = jawBottom[0];
				data[9] = jawBottom[1];

				env->SetFloatArrayRegion(arr, 0, 10, data);
				
			}

			return arr;
		}

		return nullptr;
	}

	/////////////////////////

	JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeDetectFaceWithBuffer(JNIEnv *env, jobject, jlong addr, jobject imgBuffer, jint w, jint h, jint channel, jint bytesPerRow, jobject keyPointBuffer)
	{
		CGEFaceTrackerWrapper* tracker = (CGEFaceTrackerWrapper*)addr;
		unsigned short* imgData = (unsigned short*)env->GetDirectBufferAddress(imgBuffer);
		bool suc = tracker->trackContinuousImage(imgData, w, h, channel, bytesPerRow);

		if(suc)
		{
			auto* t = tracker->getTracker();
			const auto& result = t->getPointList();
			float* keyPointData = (float*)env->GetDirectBufferAddress(keyPointBuffer);
			memcpy(keyPointData, result.data(), result.size() * sizeof(result[0]));
		}

		return suc;
	}

}

namespace CGE
{
	CGEFaceTrackerWrapper::CGEFaceTrackerWrapper() : m_tracker(nullptr)
	{
		assert(CGEFaceTracker::isTrackerSetup());
		m_tracker = new CGEFaceTracker();
	}

	CGEFaceTrackerWrapper::~CGEFaceTrackerWrapper()
	{
		delete m_tracker;
	}

	bool CGEFaceTrackerWrapper::trackContinuousImage(void* buffer, int w, int h, int channel, int stride)
	{
		if(channel == 1)
		{
			cv::Mat img(w, h, CV_8UC1, buffer, stride);
			m_hasFace = m_tracker->updateFace(img);
		}
		else
		{
			int flag0 = (channel == 3) ? CV_8UC3 : CV_8UC4;
			int flag1 = (channel == 3) ? cv::COLOR_BGR2GRAY : cv::COLOR_BGRA2GRAY;
			cv::Mat img(w, h, flag0, buffer, stride);
			cv::cvtColor(img, m_cacheImage, flag1);
			m_hasFace = m_tracker->updateFace(m_cacheImage);
		}

		return m_hasFace;
	}

	bool CGEFaceTrackerWrapper::trackImage(void* buffer, int w, int h, int stride, int channel, bool drawFeature)
	{
		long cvType = channel == 1 ? CV_8UC1 : CV_8UC4;

		cv::Mat img(h, w, cvType, buffer, stride);
		cv::Mat& grayImg = m_cacheImage;

		if(cvType != CV_8UC1)
		{
			cv::cvtColor(img, grayImg, cv::COLOR_BGRA2GRAY);
		}
		else
		{
			grayImg = img;
		}

		CGEFaceTracker::resetFrame();
		m_tracker->setWindowSize(2);

		if(!m_tracker->updateFace(grayImg))
		{
			CGE_LOG_INFO("No face...");
			m_hasFace = false;
		}
		else
		{
			
			if(drawFeature)
			{
				m_tracker->drawMeshes(img, 3, CV_RGB(0, 255, 0));

				// m_tracker->drawFeature(img, CGE_FACE_LEFT_EYEBROW, 3, false);
				// m_tracker->drawFeature(img, CGE_FACE_RIGHT_EYEBROW, 3, false);
				// m_tracker->drawFeature(img, CGE_FACE_LEFT_EYE);
				// m_tracker->drawFeature(img, CGE_FACE_RIGHT_EYE);

				// m_tracker->drawFeature(img, CGE_FACE_OUTER_MOUTH);
				// m_tracker->drawFeature(img, CGE_FACE_INNER_MOUTH);
				// m_tracker->drawFeature(img, CGE_FACE_NOSE_BRIDGE, 3, false);
				// m_tracker->drawFeature(img, CGE_FACE_NOSE_BASE);
			}
			m_hasFace = true;
			CGE_LOG_INFO("Find face...");
		}

		m_tracker->setWindowSize(0);
		return m_hasFace;
	}

}