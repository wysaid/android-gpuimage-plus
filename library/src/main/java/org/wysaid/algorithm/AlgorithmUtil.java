package org.wysaid.algorithm;

/**
 * Created by wysaid on 16/3/9.
 * Mail: admin@wysaid.org
 * blog: wysaid.org
 * Description: All classes in the algorithm directory are for OpenGL auxiliary use, so float is used extensively.
 */
public class AlgorithmUtil {
    public static float getNormalizeScaling(final float x, final float y, final float z) {
        return (float)(1.0 / Math.sqrt(x*x + y*y + z*z));
    }

    public static float getNormalizeScaling(final float x, final float y, final float z, final float w) {
        return (float)(1.0 / Math.sqrt(x*x + y*y + z*z + w*w));
    }
}
