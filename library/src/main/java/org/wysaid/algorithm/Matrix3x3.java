package org.wysaid.algorithm;

/**
 * Created by wangyang on 15/11/27.
 */
public class Matrix3x3 {
    public float[] data;

    Matrix3x3() {
        data = new float[9];
    }

    Matrix3x3(float[] _data) {
        data = _data;
    }

    public static Matrix3x3 makeIdentity() {
        return new Matrix3x3(new float[]{
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        });
    }

    public static Matrix3x3 makeRotation(float rad, float x, float y, float z) {
        final float normScaling = AlgorithmUtil.getNormalizeScaling(x, y, z);

        x *= normScaling;
        y *= normScaling;
        z *= normScaling;

        final float cosRad = (float)Math.cos(rad);
        final float cosp = 1.0f - cosRad;
        final float sinRad = (float)Math.sin(rad);

        return new Matrix3x3(new float[]{
                cosRad + cosp * x * x,
                cosp * x * y + z * sinRad,
                cosp * x * z - y * sinRad,
                cosp * x * y - z * sinRad,
                cosRad + cosp * y * y,
                cosp * y * z + x * sinRad,
                cosp * x * z + y * sinRad,
                cosp * y * z - x * sinRad,
                cosRad + cosp * z * z
        });
    }

    public static Matrix3x3 makeXRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);
        return new Matrix3x3(new float[] {
                1.0f, 0.0f, 0.0f,
                0.0f, cosRad, sinRad,
                0.0f, -sinRad, cosRad
        });
    }

    public static Matrix3x3 makeYRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);
        return new Matrix3x3(new float[] {
                cosRad, 0.0f, -sinRad,
                0.0f, 1.0f, 0.0f,
                sinRad, 0.0f, cosRad
        });
    }

    public static Matrix3x3 makeZRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);
        return new Matrix3x3(new float[] {
                cosRad, sinRad, 0.0f,
                -sinRad, cosRad, 0.0f,
                0.0f, 0.0f, 1.0f
        });
    }

    protected static float[] _mul(float[] d1, float[] d2) {
        return new float[] {
                d1[0] * d2[0] + d1[3] * d2[1] + d1[6] * d2[2],
                d1[1] * d2[0] + d1[4] * d2[1] + d1[7] * d2[2],
                d1[2] * d2[0] + d1[5] * d2[1] + d1[8] * d2[2],

                d1[0] * d2[3] + d1[3] * d2[4] + d1[6] * d2[5],
                d1[1] * d2[3] + d1[4] * d2[4] + d1[7] * d2[5],
                d1[2] * d2[3] + d1[5] * d2[4] + d1[8] * d2[5],

                d1[0] * d2[6] + d1[3] * d2[7] + d1[6] * d2[8],
                d1[1] * d2[6] + d1[4] * d2[7] + d1[7] * d2[8],
                d1[2] * d2[6] + d1[5] * d2[7] + d1[8] * d2[8]
        };
    }

    public Matrix3x3 multiply(Matrix3x3 mat) {
        return new Matrix3x3(_mul(this.data, mat.data));
    }

    public Matrix3x3 multiplyBy(Matrix3x3 mat) {
        this.data = _mul(this.data, mat.data);
        return this;
    }

    @Override
    public Matrix3x3 clone() {
        return new Matrix3x3(this.data.clone());
    }

}
