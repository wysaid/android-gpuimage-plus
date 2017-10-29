/*
* cgeFaceTrackerWrapper.h
*
*  Created on: 2016-2-17
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _cgeFaceTrackerWrapper_h_
#define _cgeFaceTrackerWrapper_h_

#include <jni.h>
#include "cgeFaceTracker.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeSetupTracker
  (JNIEnv *, jclass, jstring, jstring, jstring);

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeCreateFaceTracker
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeRelease
  (JNIEnv *, jobject, jlong);

JNIEXPORT jfloatArray JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeDetectFaceWithSimpleResult
  (JNIEnv *, jobject, jlong, jobject, jboolean);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEFaceTracker_nativeDetectFaceWithBuffer
  (JNIEnv *, jobject, jlong, jobject, jint, jint, jint, jint, jobject);

#ifdef __cplusplus
}
#endif

namespace CGE
{
	class CGEFaceTrackerWrapper
	{
	public:
		CGEFaceTrackerWrapper();
		~CGEFaceTrackerWrapper();

		bool trackContinuousImage(void* buffer, int w, int h, int channel, int stride);

		bool trackImage(void* buffer, int w, int h, int stride, int channel, bool drawFeature = false);

		inline CGEFaceTracker* getTracker() { return m_tracker; }

	private:
		CGEFaceTracker* m_tracker;
		cv::Mat m_cacheImage;
		bool m_hasFace;
	};

}

#endif