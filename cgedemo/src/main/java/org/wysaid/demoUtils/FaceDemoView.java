package org.wysaid.demoUtils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.media.FaceDetector;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Toast;

import org.wysaid.common.Common;
import org.wysaid.myUtils.ImageUtil;
import org.wysaid.nativePort.CGEFaceFunctions;

/**
 * Created by wysaid on 15/12/24.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 */

public class FaceDemoView extends View implements OnTouchListener{

    public static class FaceController {
        public Bitmap image;
        public PointF pos;
        public float rotation;
        public float halfWidth, halfHeight;
        private Matrix matrix = new Matrix();

        FaceController(Bitmap img) {
            image = img;
            pos = new PointF(0, 0);
            halfWidth = img.getWidth() / 2.0f;
            halfHeight = img.getHeight() / 2.0f;
        }

        public void drawController(Canvas canvas) {
            matrix.setTranslate(pos.x - halfWidth, pos.y - halfHeight);
            matrix.postRotate(rotation, pos.x, pos.y);
            canvas.drawBitmap(image, matrix, null);
        }

        public boolean onTouchPosition(float x, float y) {
            if(x < pos.x - halfWidth || x > pos.x + halfWidth || y < pos.y - halfHeight || y > pos.y + halfHeight)
                return false;
            return  true;
        }
    }

    private Bitmap mFaceImage;
    private FaceController mLeftEyeController, mRightEyeController;
    private FaceController mMouthController;
    private FaceController mChinController;
    private FaceController mSelectedController;
    private Matrix mMatrix = new Matrix();

    public Bitmap getFaceimage() {
        return mFaceImage;
    }

    PointF mOriginPos = new PointF(0.0f, 0.0f);
    PointF mLastFingerPos = new PointF();
    float mImageScaling = 1.0f;

    public FaceDemoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setOnTouchListener(this);
    }

    public void loadResources(Bitmap eye, Bitmap mouth, Bitmap chin) {
        mLeftEyeController = new FaceController(eye);
        mRightEyeController = new FaceController(eye);
        mMouthController = new FaceController(mouth);
        mChinController = new FaceController(chin);
    }

    public static  float getProperScaling(Bitmap bmp, View view) {
        float bmpWidth = bmp.getWidth();
        float bmpHeight = bmp.getHeight();
        float viewWidth = view.getWidth();
        float viewHeight = view.getHeight();

        return Math.min(viewWidth / bmpWidth, viewHeight / bmpHeight);
    }

    public void setFaceImage(Bitmap faceImage) {
        mFaceImage = faceImage;

        mImageScaling = getProperScaling(mFaceImage, this);

        mOriginPos.x = (this.getWidth() - mFaceImage.getWidth() * mImageScaling) / 2.0f;
        mOriginPos.y = (this.getHeight() - mFaceImage.getHeight() * mImageScaling) / 2.0f;

        ImageUtil.FaceRects faceRects = ImageUtil.findFaceByBitmap(faceImage);

        if(faceRects.numOfFaces > 0) {
            String content = "";

            FaceDetector.Face face = faceRects.faces[0];
            PointF pnt = new PointF();
            face.getMidPoint(pnt);

            float eyeDis = face.eyesDistance();
            float halfEyeDis = eyeDis / 2.0f;

            content += String.format("准确率: %g, 人脸中心 %g, %g, 眼间距: %g\n", face.confidence(), pnt.x, pnt.y, eyeDis);

            CGEFaceFunctions.FaceFeature feature = new CGEFaceFunctions.FaceFeature();

            feature.leftEyePos = new PointF(pnt.x - halfEyeDis, pnt.y);
            feature.rightEyePos = new PointF(pnt.x + halfEyeDis, pnt.y);
            feature.mouthPos = new PointF(pnt.x, pnt.y + eyeDis);
            feature.chinPos = new PointF(pnt.x, pnt.y + eyeDis * 1.5f);

            mLeftEyeController.pos = new PointF(feature.leftEyePos.x * mImageScaling + mOriginPos.x, feature.leftEyePos.y * mImageScaling + mOriginPos.y);
            mRightEyeController.pos = new PointF(feature.rightEyePos.x * mImageScaling + mOriginPos.x, feature.rightEyePos.y * mImageScaling + mOriginPos.y);
            mMouthController.pos = new PointF(feature.mouthPos.x * mImageScaling + mOriginPos.x, feature.mouthPos.y * mImageScaling + mOriginPos.y);
            mChinController.pos = new PointF(feature.chinPos.x * mImageScaling + mOriginPos.x, feature.chinPos.y * mImageScaling + mOriginPos.y);

            Toast.makeText(this.getContext(), content, Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(this.getContext(), "检测人脸失败， 情调整控件位置", Toast.LENGTH_LONG).show();
            resetControllers();
        }

        calcAngles();
        postInvalidate();
    }

    public CGEFaceFunctions.FaceFeature getFeature() {

        CGEFaceFunctions.FaceFeature feature = new CGEFaceFunctions.FaceFeature();

        feature.leftEyePos.x = (mLeftEyeController.pos.x - mOriginPos.x) / mImageScaling;
        feature.leftEyePos.y = (mLeftEyeController.pos.y - mOriginPos.y) / mImageScaling;

        feature.rightEyePos.x = (mRightEyeController.pos.x - mOriginPos.x) / mImageScaling;
        feature.rightEyePos.y = (mRightEyeController.pos.y - mOriginPos.y) / mImageScaling;

        feature.mouthPos.x = (mMouthController.pos.x - mOriginPos.x) / mImageScaling;
        feature.mouthPos.y = (mMouthController.pos.y - mOriginPos.y) / mImageScaling;

        feature.chinPos.x = (mChinController.pos.x - mOriginPos.x) / mImageScaling;
        feature.chinPos.y = (mChinController.pos.y - mOriginPos.y) / mImageScaling;

        feature.faceImageWidth = mFaceImage.getWidth();
        feature.faceImageHeight = mFaceImage.getHeight();

        return feature;
    }

    public void setFaceImageZoom(float zoom) {
        mImageScaling = zoom;
        postInvalidate();
    }

    public void resetControllers() {
        mLeftEyeController.pos.x = getWidth() / 2.0f - 100;
        mLeftEyeController.pos.y = getHeight() / 2.0f - 100;

        mRightEyeController.pos.x = getWidth() / 2.0f + 100;
        mRightEyeController.pos.y = getHeight() / 2.0f - 100;

        mMouthController.pos.x = getWidth() / 2.0f;
        mMouthController.pos.y = getHeight() / 2.0f + 50;

        mChinController.pos.x = getWidth() / 2.0f;
        mChinController.pos.y = getHeight() / 2.0f + 150;
    }

    private void calcAngles() {

        double dirX = mRightEyeController.pos.x - mLeftEyeController.pos.x;
        double dirY = mRightEyeController.pos.y - mLeftEyeController.pos.y;

        double len = Math.sqrt(dirX * dirX + dirY * dirY);
        if(len < 0.001)
            return;

        double angle = Math.asin(dirY / len);

        if(dirX >= 0.0) {
            if(dirY < 0)
                angle += 2.0 * Math.PI;

        } else {
            angle = Math.PI - angle;
        }

        float rotation = (float)(angle * 180.0 / Math.PI);

        mLeftEyeController.rotation = rotation;
        mRightEyeController.rotation = rotation;
        mMouthController.rotation = rotation;
        mChinController.rotation = rotation;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {

        float touchX = event.getX();
        float touchY = event.getY();

        int action = event.getAction();

        switch (action)
        {
            case MotionEvent.ACTION_DOWN:

                mSelectedController = null;

                if(mLeftEyeController.onTouchPosition(touchX, touchY))
                    mSelectedController = mLeftEyeController;
                else if(mRightEyeController.onTouchPosition(touchX, touchY))
                    mSelectedController = mRightEyeController;
                else if(mMouthController.onTouchPosition(touchX, touchY))
                    mSelectedController = mMouthController;
                else if(mChinController.onTouchPosition(touchX, touchY))
                    mSelectedController = mChinController;

                mLastFingerPos.x = touchX;
                mLastFingerPos.y = touchY;

                if(mSelectedController != null) {

                    mSelectedController.pos.y -= 100;
                    postInvalidate();
                    Log.i(Common.LOG_TAG, "Controller selected!");
                }
                break;
            case MotionEvent.ACTION_MOVE:
                if(mSelectedController != null) {
                    mSelectedController.pos.x += touchX - mLastFingerPos.x;
                    mSelectedController.pos.y += touchY - mLastFingerPos.y;

                    if(mSelectedController == mLeftEyeController || mSelectedController == mRightEyeController)
                        calcAngles();
                } else {
                    mOriginPos.x += touchX - mLastFingerPos.x;
                    mOriginPos.y += touchY - mLastFingerPos.y;
                }

                mLastFingerPos.x = touchX;
                mLastFingerPos.y = touchY;

                postInvalidate();

                break;
            case MotionEvent.ACTION_UP:
                mSelectedController = null;
                break;
            default:
                break;
        }

        return true;
    }

    @Override
    public void onDraw(Canvas canvas) {

        if(mFaceImage == null)
            return;

        mMatrix.setScale(mImageScaling, mImageScaling);
        mMatrix.postTranslate(mOriginPos.x, mOriginPos.y);
        canvas.drawBitmap(mFaceImage, mMatrix, null);

        mLeftEyeController.drawController(canvas);
        mRightEyeController.drawController(canvas);
        mMouthController.drawController(canvas);
        mChinController.drawController(canvas);

    }

}
