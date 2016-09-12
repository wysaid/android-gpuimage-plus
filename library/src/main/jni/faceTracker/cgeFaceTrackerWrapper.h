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

#ifdef __cplusplus
}
#endif

namespace CGE
{
	class CGEFaceTracker;

	class CGEFaceTrackerWrapper
	{
	public:
		CGEFaceTrackerWrapper();
		~CGEFaceTrackerWrapper();

		bool trackContinuousImage(void* grayBuffer, int w, int h, int stride, bool drawFeature = false);

		bool trackImage(void* buffer, int w, int h, int stride, int channel, bool drawFeature = false);

		inline CGEFaceTracker* getTracker() { return m_tracker; }

	private:
		CGEFaceTracker* m_tracker;
		bool m_hasFace;
	};

}

#endif