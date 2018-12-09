/*
* cgeDeformFilterWrapper.cpp
*
*  Created on: 2018-12-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include <cgeDeformFilterWrapper.h>
#include <CGELiquifyFilter.h>

using namespace CGE;

extern "C"
{
    JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeCreate(JNIEnv *, jobject, jint width, jint height, jfloat stride)
    {
        auto* filter = new CGELiquifyFilter();
        if(!filter->initWithMesh(width, height, stride))
        {
            delete filter;
            filter = nullptr;
        }
        return (jlong)filter;
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRelease(JNIEnv *, jobject, jlong addr)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        delete filter;
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRestore(JNIEnv *, jobject, jlong addr)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->restoreMesh();
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRestoreWithIntensity(JNIEnv *, jobject, jlong addr, jfloat intensity)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->restoreMeshWithIntensity(intensity);
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeForwardDeform(JNIEnv *, jobject, jlong addr, jfloat startX, jfloat startY, jfloat endX, jfloat endY, jfloat w, jfloat h, jfloat radius, jfloat intensity)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->forwardDeformMesh(Vec2f(startX, startY), Vec2f(endX, endY), w, h, radius, intensity);
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRestoreWithPoint(JNIEnv *, jobject, jlong addr, jfloat x, jfloat y, jfloat w, jfloat h, jfloat radius, jfloat intensity)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->restoreMeshWithPoint(Vec2f(x, y), w, h, radius, intensity);
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeBloatDeform(JNIEnv *, jobject, jlong addr, jfloat x, jfloat y, jfloat w, jfloat h, jfloat radius, jfloat intensity)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->bloatMeshWithPoint(Vec2f(x, y), w, h, radius, intensity);
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeWrinkleDeform(JNIEnv *, jobject, jlong addr, jfloat x, jfloat y, jfloat w, jfloat h, jfloat radius, jfloat intensity)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->wrinkleMeshWithPoint(Vec2f(x, y), w, h, radius, intensity);
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeSetUndoSteps(JNIEnv *, jobject, jlong addr, jint steps)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->setUndoSteps(steps);
    }

    JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeCanUndo(JNIEnv *, jobject, jlong addr)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        return filter->canUndo();
    }

    JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeCanRedo(JNIEnv *, jobject, jlong addr)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        return filter->canRedo();
    }

    JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeUndo(JNIEnv *, jobject, jlong addr)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        return filter->undo();
    }

    JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeRedo(JNIEnv *, jobject, jlong addr)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        return filter->redo();
    }

    JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativePushDeformStep(JNIEnv *, jobject, jlong addr)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        return filter->pushMesh();
    }

    JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEDeformFilterWrapper_nativeShowMesh(JNIEnv *, jobject, jlong addr, jboolean show)
    {
        auto* filter = (CGELiquifyFilter*)addr;
        filter->showMesh(show);
    }
}