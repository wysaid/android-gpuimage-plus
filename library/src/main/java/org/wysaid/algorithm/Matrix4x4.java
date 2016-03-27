package org.wysaid.algorithm;

/**
 * Created by wangyang on 15/11/27.
 */
public class Matrix4x4 {
    public float[] data;

    Matrix4x4() {
        data = new float[16];
    }

    Matrix4x4(float[] _data) {
        data = _data;
    }

    public static Matrix4x4 makeIdentity() {
        return new Matrix4x4(new float[]{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        });
    }

    public static Matrix4x4 makeRotation(float rad, float x, float y, float z) {
        final float normScaling = AlgorithmUtil.getNormalizeScaling(x, y, z);

        x *= normScaling;
        y *= normScaling;
        z *= normScaling;

        final float cosRad = (float)Math.cos(rad);
        final float cosp = 1.0f - cosRad;
        final float sinRad = (float)Math.sin(rad);

        return new Matrix4x4(new float[]{
                cosRad + cosp * x * x,
                cosp * x * y + z * sinRad,
                cosp * x * z - y * sinRad,
                0.0f,
                cosp * x * y - z * sinRad,
                cosRad + cosp * y * y,
                cosp * y * z + x * sinRad,
                0.0f,
                cosp * x * z + y * sinRad,
                cosp * y * z - x * sinRad,
                cosRad + cosp * z * z,
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                1.0f
        });
    }

    public static Matrix4x4 makeXRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);
        return new Matrix4x4(new float[]{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, cosRad, sinRad, 0.0f,
                0.0f, -sinRad, cosRad, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        });
    }

    public static Matrix4x4 makeYRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);
        return new Matrix4x4(new float[]{
                cosRad, 0.0f, -sinRad, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                sinRad, 0.0f, cosRad, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        });
    }

    public static Matrix4x4 makeZRotation(float rad) {
        final float cosRad = (float)Math.cos(rad);
        final float sinRad = (float)Math.sin(rad);
        return new Matrix4x4(new float[]{
                cosRad, sinRad, 0.0f, 0.0f,
                -sinRad, cosRad, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        });
    }

    public static Matrix4x4 makeTranslation(float x, float y, float z) {
        return new Matrix4x4(new float[]{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                x, y, z, 1.0f
        });
    }

    public static Matrix4x4 makeScaling(float x, float y, float z) {
        return new Matrix4x4(new float[]{
                x, 0.0f, 0.0f, 0.0f,
                0.0f, y, 0.0f, 0.0f,
                0.0f, 0.0f, z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        });
    }

    public static Matrix4x4 makePerspective(float fovyRad, float aspect, float nearZ, float farZ) {

        final float cotan = 1.0f / (float)Math.tan(fovyRad / 2.0f);

        return new Matrix4x4(new float[]{
                cotan / aspect, 0.0f, 0.0f, 0.0f,
                0.0f, cotan, 0.0f, 0.0f,
                0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
                0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f
        });
    }

    public static Matrix4x4 makeFrustum(float left, float right, float bottom, float top, float nearZ, float farZ) {

        final float ral = right + left;
        final float rsl = right - left;
        final float tsb = top - bottom;
        final float tab = top + bottom;
        final float fan = farZ + nearZ;
        final float fsn = farZ - nearZ;

        return new Matrix4x4(new float[]{
                2.0f * nearZ / rsl, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f * nearZ / tsb, 0.0f, 0.0f,
                ral / rsl, tab / tsb, -fan / fsn, -1.0f,
                0.0f, 0.0f, (-2.0f * farZ * nearZ) / fsn, 0.0f
        });
    }

    public static Matrix4x4 makeOrtho(float left, float right, float bottom, float top, float nearZ, float farZ)
    {
        final float ral = right + left;
        final float rsl = right - left;
        final float tsb = top - bottom;
        final float tab = top + bottom;
        final float fan = farZ + nearZ;
        final float fsn = farZ - nearZ;

        return new Matrix4x4(new float[]{
                2.0f / rsl, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / tsb, 0.0f, 0.0f,
                0.0f, 0.0f, -2.0f / fsn, 0.0f,
                -ral / rsl, -tab / tsb, -fan / fsn, 1.0f
        });
    }

//    public static Matrix4x4 makeLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ,	float upX, float upY, float upZ)
//    {
//        Vec3f ev(eyeX, eyeY, eyeZ);
//        Vec3f cv(centerX, centerY, centerZ);
//        Vec3f uv(upX, upY, upZ);
//        return makeLookAt(ev, cv, uv);
//    }
//
//    static inline Mat4 makeLookAt(const Vec3f& eye, const Vec3f& center, const Vec3f& up)
//    {
//        Vec3f forward((eye - center).normalize());
//        Vec3f side(crossV3f(up, forward).normalize());
//        Vec3f upVector(crossV3f(forward, side));
//
//        return Mat4(side[0], upVector[0], forward[0], 0.0f,
//                side[1], upVector[1], forward[1], 0.0f,
//                side[2], upVector[2], forward[2], 0.0f,
//                -side.dot(eye),
//                -upVector.dot(eye),
//                -forward.dot(eye),
//                1.0f);
//    }

    protected static float[] _mul(float[] d1, float[] d2) {
        return new float[]{
                d1[0] * d2[0] + d1[4] * d2[1] + d1[8] * d2[2] + d1[12] * d2[3],
                d1[1] * d2[0] + d1[5] * d2[1] + d1[9] * d2[2] + d1[13] * d2[3],
                d1[2] * d2[0] + d1[6] * d2[1] + d1[10] * d2[2] + d1[14] * d2[3],
                d1[3] * d2[0] + d1[7] * d2[1] + d1[11] * d2[2] + d1[15] * d2[3],
                d1[0] * d2[4] + d1[4] * d2[5] + d1[8] * d2[6] + d1[12] * d2[7],
                d1[1] * d2[4] + d1[5] * d2[5] + d1[9] * d2[6] + d1[13] * d2[7],
                d1[2] * d2[4] + d1[6] * d2[5] + d1[10] * d2[6] + d1[14] * d2[7],
                d1[3] * d2[4] + d1[7] * d2[5] + d1[11] * d2[6] + d1[15] * d2[7],
                d1[0] * d2[8] + d1[4] * d2[9] + d1[8] * d2[10] + d1[12] * d2[11],
                d1[1] * d2[8] + d1[5] * d2[9] + d1[9] * d2[10] + d1[13] * d2[11],
                d1[2] * d2[8] + d1[6] * d2[9] + d1[10] * d2[10] + d1[14] * d2[11],
                d1[3] * d2[8] + d1[7] * d2[9] + d1[11] * d2[10] + d1[15] * d2[11],
                d1[0] * d2[12] + d1[4] * d2[13] + d1[8] * d2[14] + d1[12] * d2[15],
                d1[1] * d2[12] + d1[5] * d2[13] + d1[9] * d2[14] + d1[13] * d2[15],
                d1[2] * d2[12] + d1[6] * d2[13] + d1[10] * d2[14] + d1[14] * d2[15],
                d1[3] * d2[12] + d1[7] * d2[13] + d1[11] * d2[14] + d1[15] * d2[15]
        };
    }

    public Matrix4x4 multiply(Matrix4x4 mat) {
        return new Matrix4x4(_mul(this.data, mat.data));
    }

    public Matrix4x4 multiplyBy(Matrix4x4 mat) {
        this.data = _mul(this.data, mat.data);
        return this;
    }

    @Override
    public Matrix4x4 clone() {
        return new Matrix4x4(this.data.clone());
    }

}
