package org.wysaid.gpuCodec;

import org.wysaid.common.TextureDrawer;

/**
 * Author: wangyang
 * Mail: admin@wysaid.org
 * Date: 2017/5/3
 * Description:
 */

public class TextureDrawerCodec extends TextureDrawer {

    static final float[] MATRIX_YUV2RGB = new float[] {
            1.0f, 1.0f, 1.0f,
            0.0f, -0.18732f, 1.8556f,
            1.57481f, -0.46813f, 0.0f
    };

    //Invert matrix for 'MATRIX_YUV2RGB'
    static final float[] MATRIX_RGB2YUV = new float[] {
            0.21260134f, -0.11457283f,  0.49999598f,
            0.71520028f, -0.38542805f, -0.4541502f,
            0.07219838f,  0.50000087f, -0.04584577f,
    };

    public static final String COLOR_CONVERSION_NAME = "colorConversion";

}
