/*
 * cgeBlendFilter.cpp
 *
 *  Created on: 2015-2-13
 *      Author: Wang Yang
 */

#include "cgeBlendFilter.h"

#include <string>

#define BLEND_TEXTURE_BIND_ID 0
#define BUFFER_LEN 4096

// clang-format off

static CGEConstString s_blendHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D blendTexture;
    uniform float intensity;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        vec4 dst = texture2D(blendTexture, textureCoordinate);
        gl_FragColor = vec4(blend(src.rgb, dst.rgb, dst.a * intensity), src.a);
    });

static CGEConstString s_blendKeepRatioHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D blendTexture;
    uniform float intensity;
    uniform vec4 ratioAspect;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        vec4 dst = texture2D(blendTexture, (textureCoordinate * ratioAspect.xy) + ratioAspect.zw);
        gl_FragColor = vec4(blend(src.rgb, dst.rgb, dst.a * intensity), src.a);
    });

static CGEConstString s_blendTileHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D blendTexture;
    uniform float intensity;
    uniform vec2 scalingRatio;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        vec4 dst = texture2D(blendTexture, fract(textureCoordinate * scalingRatio));
        gl_FragColor = vec4(blend(src.rgb, dst.rgb, dst.a * intensity), src.a);
    });

static CGEConstString s_blendPixelHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec4 blendColor;
    uniform float intensity;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        gl_FragColor = vec4(blend(src.rgb, blendColor.rgb, intensity * blendColor.a), src.a);
    });

static CGEConstString s_blendWithSelfHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform float intensity;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        gl_FragColor = vec4(blend(src.rgb, src.rgb, intensity), src.a);
    });

static CGEConstString s_blendVignetteHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec4 blendColor;
    uniform float intensity;

    uniform vec2 vignette;
    uniform vec2 vignetteCenter;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);

        gl_FragColor = vec4(blend(src.rgb, blendColor.rgb, intensity * blendColor.a * percent), src.a);
    });

static CGEConstString s_blendVignette2HelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec4 blendColor;
    uniform float intensity;

    uniform vec2 vignette;
    uniform vec2 vignetteCenter;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        percent = percent * percent * (3.0 - 2.0 * percent);

        gl_FragColor = vec4(blend(src.rgb, blendColor.rgb, intensity * blendColor.a * percent), src.a);
    });

///////////////////////////////////////////////////////////////////////////

static CGEConstString s_blendVignetteNoAlphaHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec4 blendColor;
    uniform float intensity;

    uniform vec2 vignette;
    uniform vec2 vignetteCenter;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent;

        gl_FragColor = vec4(blend(src.rgb, blendColor.rgb * alpha, intensity * blendColor.a), src.a);
    });

static CGEConstString s_blendVignette2NoAlphaHelpStart = CGE_SHADER_STRING_PRECISION_H(
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform vec4 blendColor;
    uniform float intensity;

    uniform vec2 vignette;
    uniform vec2 vignetteCenter;

    %s\n

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);

        float d = distance(textureCoordinate, vignetteCenter);
        float percent = clamp((d - vignette.x) / vignette.y, 0.0, 1.0);
        float alpha = 1.0 - percent * percent * (3.0 - 2.0 * percent);

        gl_FragColor = vec4(blend(src.rgb, blendColor.rgb * alpha, intensity * blendColor.a), src.a);
    });

//////////////////////////////////////////////////////////////////////////

static CGEConstString s_hardLight = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        if (src2.r < 0.5)
            src2.r = (src1.r * src2.r) * 2.0;
        else
            src2.r = (src1.r + src2.r) * 2.0 - (src1.r * src2.r) * 2.0 - 1.0;

        if (src2.g < 0.5)
            src2.g = (src1.g * src2.g) * 2.0;
        else
            src2.g = (src1.g + src2.g) * 2.0 - (src1.g * src2.g) * 2.0 - 1.0;

        if (src2.b < 0.5)
            src2.b = (src1.b * src2.b) * 2.0;
        else
            src2.b = (src1.b + src2.b) * 2.0 - (src1.b * src2.b) * 2.0 - 1.0;

        return mix(src1, src2, alpha);
    });

static CGEConstString s_overLay = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        if (src1.r < 0.5)
            src2.r = (src1.r * src2.r) * 2.0;
        else
            src2.r = (src1.r + src2.r) * 2.0 - (src1.r * src2.r) * 2.0 - 1.0;

        if (src1.g < 0.5)
            src2.g = (src1.g * src2.g) * 2.0;
        else
            src2.g = (src1.g + src2.g) * 2.0 - (src1.g * src2.g) * 2.0 - 1.0;

        if (src1.b < 0.5)
            src2.b = (src1.b * src2.b) * 2.0;
        else
            src2.b = (src1.b + src2.b) * 2.0 - (src1.b * src2.b) * 2.0 - 1.0;

        return mix(src1, src2, alpha);
    });

static CGEConstString s_softLight = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        if (src2.r < 0.5)
            src2.r = (src2.r * 2.0 - 1.0) * (src1.r - (src1.r * src1.r)) + src1.r;
        else
            src2.r = ((src2.r * 2.0 - 1.0) * (sqrt(src1.r) - src1.r)) + src1.r;

        if (src2.g < 0.5)
            src2.g = (src2.g * 2.0 - 1.0) * (src1.g - (src1.g * src1.g)) + src1.g;
        else
            src2.g = ((src2.g * 2.0 - 1.0) * (sqrt(src1.g) - src1.g)) + src1.g;

        if (src2.b < 0.5)
            src2.b = (src2.b * 2.0 - 1.0) * (src1.b - (src1.b * src1.b)) + src1.b;
        else
            src2.b = ((src2.b * 2.0 - 1.0) * (sqrt(src1.b) - src1.b)) + src1.b;

        return mix(src1, src2, alpha);
    });

static CGEConstString s_screen = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, src1 + src2 - src1 * src2, alpha);
    });

static CGEConstString s_linearLight = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, clamp(src1 + src2 * 2.0 - 1.0, 0.0, 1.0), alpha);
    });

static CGEConstString s_vividLight = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        src2.r = src2.r < 0.5 ? 1.0 - (1.0 - src1.r) / (src2.r * 2.0) : src1.r / (1.0 - src2.r) * 0.5;
        src2.g = src2.g < 0.5 ? 1.0 - (1.0 - src1.g) / (src2.g * 2.0) : src1.g / (1.0 - src2.g) * 0.5;
        src2.b = src2.b < 0.5 ? 1.0 - (1.0 - src1.b) / (src2.b * 2.0) : src1.b / (1.0 - src2.b) * 0.5;

        return mix(src1, clamp(src2, 0.0, 1.0), alpha);
    });

static CGEConstString s_multiply = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, src1 * src2, alpha);
    });

static CGEConstString s_exclude = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, src1 + src2 - src1 * src2 * 2.0, alpha);
    });

static CGEConstString s_darken = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, min(src1, src2), alpha);
    });

static CGEConstString s_lighten = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, max(src1, src2), alpha);
    });

static CGEConstString s_colorBurn = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        /// plus 0.001 to avoid dividing by float zero
        return mix(src1, 1.0 - min((1.0 - src1) / (src2 + 0.001), 1.0), alpha);
    });

static CGEConstString s_colorDodge = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, min(src1 / (1.0 - src2), 1.0), alpha);
    });

// static CGEConstString s_colorDodgeAdobe = CGE_SHADER_STRING
// (
// vec3 blend(vec3 src1, vec3 src2, float alpha)
// {
// 	return mix(src1, min(src1 + (src1 * src2) / (1.0 - src2), 1.0), alpha);
// }
// );

static CGEConstString s_linearDodge = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, min(src1 + src2, 1.0), alpha);
    });

static CGEConstString s_linearBurn = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, max(src1 + src2 - 1.0, 0.0), alpha);
    });

static CGEConstString s_pinLight = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        src2 *= 2.0;
        if (src2.r > src1.r)
        {
            src2.r = src2.r - 1.0;
            if (src2.r < src1.r)
                src2.r = src1.r;
        }
        if (src2.g > src1.g)
        {
            src2.g = src2.g - 1.0;
            if (src2.g < src1.g)
                src2.g = src1.g;
        }
        if (src2.b > src1.b)
        {
            src2.b = src2.b - 1.0;
            if (src2.b < src1.b)
                src2.b = src1.b;
        }

        return mix(src1, src2, alpha);
    });

static CGEConstString s_hardMix = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, step(1.0 - src1, src2), alpha);
    });

static CGEConstString s_difference = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, abs(src1 - src2), alpha);
    });

static CGEConstString s_add = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return src1 + src2 * alpha; // mix(src1, src1 + src2, alpha);
    });

static CGEConstString s_mix = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, src2, alpha);
    });

static CGEConstString s_color = CGE_SHADER_STRING(
    float getLumValue(vec3 src) {
        return 0.299 * src.r + 0.587 * src.g + 0.114 * src.b;
    }

    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, getLumValue(src1) - getLumValue(src2) + src2, alpha);
    });

static CGEConstString s_addReverse = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return src1 * (1.0 - alpha) + src2;
    });

static CGEConstString s_colorBW = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return (src1 * (1.0 - alpha) + dot(src1, vec3(0.299, 0.587, 0.114)) * alpha);
    });

static CGEConstString s_dissolve = CGE_SHADER_STRING(
    float random(vec2 seed) {
        return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
    } vec3 blend(vec3 src1, vec3 src2, float alpha) {
        vec2 seed = textureCoordinate + (src1.rg + src2.rg + vec2(src1.b, src2.b)) * 10.0;
        return mix(src2, src1, step(alpha, random(seed)));
    });

static CGEConstString s_darkerColor = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        vec3 dst = dot(src1, vec3(0.299, 0.587, 0.114)) < dot(src2, vec3(0.299, 0.587, 0.114)) ? src1 : src2;
        return mix(src1, dst, alpha);
    });

static CGEConstString s_lighterColor = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        vec3 dst = dot(src1, vec3(0.299, 0.587, 0.114)) > dot(src2, vec3(0.299, 0.587, 0.114)) ? src1 : src2;
        return mix(src1, dst, alpha);
    });

static CGEConstString s_subtract = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, max(src1 - src2, 0.0), alpha);
    });

static CGEConstString s_divide = CGE_SHADER_STRING(
    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, min(src1 / src2, 1.0), alpha);
    });

static CGEConstString s_hue = CGE_SHADER_STRING(

    vec3 RGB2HSL(vec3 src) {
        float maxc = max(max(src.r, src.g), src.b);
        float minc = min(min(src.r, src.g), src.b);
        float L = (maxc + minc) / 2.0;
        if (maxc == minc)
            return vec3(0.0, 0.0, L);
        float H, S;

        if (L < 0.5)
            S = (maxc - minc) / (maxc + minc);
        else
            S = (maxc - minc) / (2.0 - maxc - minc);

        if (maxc == src.r)
            H = (src.g - src.b) / (maxc - minc);
        else if (maxc == src.g)
            H = 2.0 + (src.b - src.r) / (maxc - minc);
        else
            H = 4.0 + (src.r - src.g) / (maxc - minc);
        H *= 60.0;
        if (H < 0.0) H += 360.0;
        return vec3(H / 360.0, S, L); // H(0~1), S(0~1), L(0~1)
    }

    vec3 HSL2RGB(vec3 src) // H, S, L
    {
        if (src.y <= 0.0)
            return vec3(src.z, src.z, src.z);
        float q = (src.z < 0.5) ? src.z * (1.0 + src.y) : (src.z + src.y - (src.y * src.z));
        float p = 2.0 * src.z - q;

        vec3 dst = vec3(src.x + 0.333, src.x, src.x - 0.333);

        if (dst.r < 0.0)
            dst.r += 1.0;
        else if (dst.r > 1.0)
            dst.r -= 1.0;

        if (dst.g < 0.0)
            dst.g += 1.0;
        else if (dst.g > 1.0)
            dst.g -= 1.0;

        if (dst.b < 0.0)
            dst.b += 1.0;
        else if (dst.b > 1.0)
            dst.b -= 1.0;

        if (dst.r < 1.0 / 6.0)
            dst.r = p + (q - p) * 6.0 * dst.r;
        else if (dst.r < 0.5)
            dst.r = q;
        else if (dst.r < 2.0 / 3.0)
            dst.r = p + (q - p) * ((2.0 / 3.0) - dst.r) * 6.0;
        else
            dst.r = p;

        if (dst.g < 1.0 / 6.0)
            dst.g = p + (q - p) * 6.0 * dst.g;
        else if (dst.g < 0.5)
            dst.g = q;
        else if (dst.g < 2.0 / 3.0)
            dst.g = p + (q - p) * ((2.0 / 3.0) - dst.g) * 6.0;
        else
            dst.g = p;

        if (dst.b < 1.0 / 6.0)
            dst.b = p + (q - p) * 6.0 * dst.b;
        else if (dst.b < 0.5)
            dst.b = q;
        else if (dst.b < 2.0 / 3.0)
            dst.b = p + (q - p) * ((2.0 / 3.0) - dst.b) * 6.0;
        else
            dst.b = p;

        return dst;
    }

    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        vec3 hsl1 = RGB2HSL(src1);
        vec3 hsl2 = RGB2HSL(src2);
        vec3 dst = HSL2RGB(vec3(hsl2.r, hsl1.gb));

        return mix(src1, dst, alpha);
    });

static CGEConstString s_saturation = CGE_SHADER_STRING(
    vec3 RGB2HSL(vec3 src) {
        float maxc = max(max(src.r, src.g), src.b);
        float minc = min(min(src.r, src.g), src.b);
        float L = (maxc + minc) / 2.0;
        if (maxc == minc)
            return vec3(0.0, 0.0, L);
        float H, S;

        if (L < 0.5)
            S = (maxc - minc) / (maxc + minc);
        else
            S = (maxc - minc) / (2.0 - maxc - minc);

        if (maxc == src.r)
            H = (src.g - src.b) / (maxc - minc);
        else if (maxc == src.g)
            H = 2.0 + (src.b - src.r) / (maxc - minc);
        else
            H = 4.0 + (src.r - src.g) / (maxc - minc);
        H *= 60.0;
        if (H < 0.0) H += 360.0;
        return vec3(H / 360.0, S, L); // H(0~1), S(0~1), L(0~1)
    }

    vec3 HSL2RGB(vec3 src) // H, S, L
    {
        if (src.y <= 0.0)
            return vec3(src.z, src.z, src.z);
        float q = (src.z < 0.5) ? src.z * (1.0 + src.y) : (src.z + src.y - (src.y * src.z));
        float p = 2.0 * src.z - q;

        vec3 dst = vec3(src.x + 0.333, src.x, src.x - 0.333);

        if (dst.r < 0.0)
            dst.r += 1.0;
        else if (dst.r > 1.0)
            dst.r -= 1.0;

        if (dst.g < 0.0)
            dst.g += 1.0;
        else if (dst.g > 1.0)
            dst.g -= 1.0;

        if (dst.b < 0.0)
            dst.b += 1.0;
        else if (dst.b > 1.0)
            dst.b -= 1.0;

        if (dst.r < 1.0 / 6.0)
            dst.r = p + (q - p) * 6.0 * dst.r;
        else if (dst.r < 0.5)
            dst.r = q;
        else if (dst.r < 2.0 / 3.0)
            dst.r = p + (q - p) * ((2.0 / 3.0) - dst.r) * 6.0;
        else
            dst.r = p;

        if (dst.g < 1.0 / 6.0)
            dst.g = p + (q - p) * 6.0 * dst.g;
        else if (dst.g < 0.5)
            dst.g = q;
        else if (dst.g < 2.0 / 3.0)
            dst.g = p + (q - p) * ((2.0 / 3.0) - dst.g) * 6.0;
        else
            dst.g = p;

        if (dst.b < 1.0 / 6.0)
            dst.b = p + (q - p) * 6.0 * dst.b;
        else if (dst.b < 0.5)
            dst.b = q;
        else if (dst.b < 2.0 / 3.0)
            dst.b = p + (q - p) * ((2.0 / 3.0) - dst.b) * 6.0;
        else
            dst.b = p;

        return dst;
    }

    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        vec3 hsl1 = RGB2HSL(src1);
        vec3 hsl2 = RGB2HSL(src2);
        vec3 dst = HSL2RGB(vec3(hsl1.r, hsl2.g, hsl1.b));

        return mix(src1, dst, alpha);
    });

static CGEConstString s_luminosity = CGE_SHADER_STRING(
    float getLumValue(vec3 src) {
        return 0.299 * src.r + 0.587 * src.g + 0.114 * src.b;
    }

    vec3 blend(vec3 src1, vec3 src2, float alpha) {
        return mix(src1, getLumValue(src2) - getLumValue(src1) + src1, alpha);
    });

// clang-format on

// 顺序必须与 CGETextureBlendMode 里面的模式匹配， 否则将造成效果不正确
static const char* s_shaderEnum[] = {
    s_mix,      // 0 正常
    s_dissolve, // 1 溶解

    s_darken,      // 2 变暗
    s_multiply,    // 3 正片叠底
    s_colorBurn,   // 4 颜色加深
    s_linearBurn,  // 5 线性加深
    s_darkerColor, // 6 深色

    s_lighten,      // 7 变亮
    s_screen,       // 8 滤色
    s_colorDodge,   // 9 颜色减淡
    s_linearDodge,  // 10 线性减淡
    s_lighterColor, // 11 浅色

    s_overLay,     // 12 叠加
    s_softLight,   // 13 柔光
    s_hardLight,   // 14 强光
    s_vividLight,  // 15 亮光
    s_linearLight, // 16 线性光
    s_pinLight,    // 17 点光
    s_hardMix,     // 18 实色混合

    s_difference, // 19 差值
    s_exclude,    // 20 排除
    s_subtract,   // 21 减去
    s_divide,     // 22 划分
    s_hue,        // 23 色相
    s_saturation, // 24 饱和度
    s_color,      // 25 颜色
    s_luminosity, // 26 明度

    /////////////    Special blend mode below     //////////////

    s_add,
    s_addReverse,
    s_colorBW,
};

const int s_nShaderEnum = sizeof(s_shaderEnum) / sizeof(*s_shaderEnum);

namespace CGE
{
CGEConstString CGEBlendInterface::paramIntensityName = "intensity";

CGETextureBlendMode CGEBlendInterface::getBlendModeByName(const char* modeName)
{
    std::string s(modeName);

    //依照枚举顺序列举

    if (s == "mix")
    {
        return CGE_BLEND_MIX;
    }
    else if (s == "dsv" || s == "dissolve")
    {
        return CGE_BLEND_DISSOLVE;
    }
    else if (s == "dk" || s == "darken")
    {
        return CGE_BLEND_DARKEN;
    }
    else if (s == "mp" || s == "multiply")
    {
        return CGE_BLEND_MULTIPLY;
    }
    else if (s == "cb" || s == "colorburn")
    {
        return CGE_BLEND_COLORBURN;
    }
    else if (s == "lb" || s == "linearburn")
    {
        return CGE_BLEND_LINEARBURN;
    }
    else if (s == "dc" || s == "darkercolor")
    {
        return CGE_BLEND_DARKER_COLOR;
    }
    else if (s == "lt" || s == "lighten")
    {
        return CGE_BLEND_LIGHTEN;
    }
    else if (s == "sr" || s == "screen")
    {
        return CGE_BLEND_SCREEN;
    }
    else if (s == "cd" || s == "colordodge")
    {
        return CGE_BLEND_COLORDODGE;
    }
    else if (s == "ld" || s == "lineardodge")
    {
        return CGE_BLEND_LINEARDODGE;
    }
    else if (s == "lc" || s == "lightercolor")
    {
        return CGE_BLEND_LIGHTERCOLOR;
    }
    else if (s == "ol" || s == "overlay")
    {
        return CGE_BLEND_OVERLAY;
    }
    else if (s == "sl" || s == "softlight")
    {
        return CGE_BLEND_SOFTLIGHT;
    }
    else if (s == "hl" || s == "hardlight")
    {
        return CGE_BLEND_HARDLIGHT;
    }
    else if (s == "vvl" || s == "vividlight")
    {
        return CGE_BLEND_VIVIDLIGHT;
    }
    else if (s == "ll" || s == "linearlight")
    {
        return CGE_BLEND_LINEARLIGHT;
    }
    else if (s == "pl" || s == "pinlight")
    {
        return CGE_BLEND_PINLIGHT;
    }
    else if (s == "hm" || s == "hardmix")
    {
        return CGE_BLEND_HARDMIX;
    }
    else if (s == "dif" || s == "difference")
    {
        return CGE_BLEND_DIFFERENCE;
    }
    else if (s == "ec" || s == "exclude")
    {
        return CGE_BLEND_EXCLUDE;
    }
    else if (s == "sub" || s == "subtract")
    {
        return CGE_BLEND_SUBTRACT;
    }
    else if (s == "div" || s == "divide")
    {
        return CGE_BLEND_DIVIDE;
    }
    else if (s == "hue")
    {
        return CGE_BLEND_HUE;
    }
    else if (s == "sat" || s == "saturation")
    {
        return CGE_BLEND_SATURATION;
    }
    else if (s == "cl" || s == "color")
    {
        return CGE_BLEND_COLOR;
    }
    else if (s == "lum" || s == "luminosity")
    {
        return CGE_BLEND_LUMINOSITY;
    }
    else if (s == "add")
    {
        return CGE_BLEND_ADD;
    }
    else if (s == "addrev")
    {
        return CGE_BLEND_ADDREV;
    }
    else if (s == "colorbw")
    {
        return CGE_BLEND_COLORBW;
    }

    CGE_LOG_ERROR("Invalid mode name: %s\n", modeName);
    return CGE_BLEND_MIX;
}

const char* CGEBlendInterface::getShaderFuncByBlendMode(const char* modeName)
{
    return getShaderFuncByBlendMode(getBlendModeByName(modeName));
}

const char* CGEBlendInterface::getShaderFuncByBlendMode(CGETextureBlendMode mode)
{
    if (mode < 0 || mode >= s_nShaderEnum) return nullptr;
    return s_shaderEnum[mode];
}

const char* CGEBlendInterface::getBlendWrapper()
{
    return s_blendHelpStart;
}

const char* CGEBlendInterface::getBlendKrWrapper()
{
    return s_blendKeepRatioHelpStart;
}

const char* CGEBlendInterface::getBlendPixWrapper()
{
    return s_blendPixelHelpStart;
}

const char* CGEBlendInterface::getBlendSelfWrapper()
{
    return s_blendWithSelfHelpStart;
}

bool CGEBlendInterface::initWithModeName(const char* modeName, CGEBlendInterface* blendIns)
{
    CGETextureBlendMode mode = getBlendModeByName(modeName);
    return blendIns->initWithMode(mode);
}

void CGEBlendInterface::setIntensity(float value)
{
    m_program.bind();
    m_program.sendUniformf(paramIntensityName, value);
}

//////////////////////////////////////////////////////////////////////////

void CGEBlendFilter::initSampler()
{
    UniformParameters* param;
    if (m_uniformParam == nullptr)
        param = new UniformParameters;
    else
        param = m_uniformParam;
    param->pushSampler2D(paramBlendTextureName, &m_blendTexture, BLEND_TEXTURE_BIND_ID);
    setAdditionalUniformParameter(param);
}

bool CGEBlendFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        initSampler();
        return true;
    }
    CGE_LOG_ERROR("Init CGEBlendFilter failed, Mode number %d\n", mode);
    return false;
}

bool CGEBlendFilter::initWithMode(const char* modeName)
{
    return initWithModeName(modeName, this);
}

void CGEBlendFilter::setSamplerID(GLuint texID, bool shouldDelete)
{
    if (texID == m_blendTexture)
        return;
    if (shouldDelete)
        glDeleteTextures(1, &m_blendTexture);
    m_blendTexture = texID;
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGEBlendFilter::paramBlendTextureName = "blendTexture";

void CGEBlendWithResourceFilter::setTexSize(int w, int h)
{
    m_blendTextureSize.set(w, h);
}

CGEConstString CGEBlendKeepRatioFilter::paramAspectRatioName = "ratioAspect";

bool CGEBlendKeepRatioFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendKeepRatioHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        initSampler();
        if (m_uniformParam != nullptr)
            m_uniformParam->requireRatioAspect(paramAspectRatioName);
        return true;
    }
    CGE_LOG_ERROR("Init CGEBlendFilter failed, Mode number %d\n", mode);
    return false;
}

bool CGEBlendKeepRatioFilter::initWithMode(const char* modeName)
{
    return initWithModeName(modeName, this);
}

void CGEBlendKeepRatioFilter::setTexSize(int w, int h)
{
    m_blendTextureSize.set(w, h);
    flushTexSize();
}

void CGEBlendKeepRatioFilter::flushTexSize()
{
    if (!m_uniformParam)
        return;
    UniformParameters::UniformData* data = m_uniformParam->getDataPointerByName(paramAspectRatioName);
    data->uniformValue[0].valuef = m_blendTextureSize.width / (float)m_blendTextureSize.height;
}

CGEConstString CGEBlendTileFilter::paramScalingRatioName = "scalingRatio";

bool CGEBlendTileFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendTileHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        initSampler();
        return true;
    }
    CGE_LOG_ERROR("Init BlendProcessor failed, Mode number %d\n", mode);
    return false;
}

bool CGEBlendTileFilter::initWithMode(const char* modeName)
{
    return initWithModeName(modeName, this);
}

void CGEBlendTileFilter::render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
{
    handler->setAsTarget();
    m_program.bind();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);

    const CGESizei& sz = handler->getOutputFBOSize();
    m_program.sendUniformf(paramScalingRatioName, sz.width / (float)m_blendTextureSize.width, sz.height / (float)m_blendTextureSize.height);

    if (m_uniformParam != nullptr)
        m_uniformParam->assignUniforms(handler, m_program.programID());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    cgeCheckGLError("glDrawArrays");
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGEPixblendFilter::paramBlendColorName = "blendColor";

void CGEPixblendFilter::setBlendColor(float r, float g, float b, float a)
{
    m_program.bind();
    m_program.sendUniformf(paramBlendColorName, r, g, b, a);
}

bool CGEPixblendFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendPixelHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        return true;
    }
    CGE_LOG_ERROR("Init CGEPixblendFilter failed, Mode number %d\n", mode);
    return false;
}

bool CGEPixblendFilter::initWithMode(const char* modeName)
{
    return initWithModeName(modeName, this);
}

//////////////////////////////////////////////////////////////////////////

bool CGEBlendWithSelfFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendWithSelfHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        return true;
    }
    CGE_LOG_ERROR("Init CGEBlendWithSelfFilter failed, Mode number %d\n", mode);
    return false;
}

bool CGEBlendWithSelfFilter::initWithMode(const char* modeName)
{
    return initWithModeName(modeName, this);
}

//////////////////////////////////////////////////////////////////////////

CGEConstString CGEBlendVignetteFilter::paramVignetteCenterName = "vignetteCenter";
CGEConstString CGEBlendVignetteFilter::paramVignetteName = "vignette";

bool CGEBlendVignetteFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendVignetteHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        setVignetteCenter(0.5, 0.5);
        return true;
    }
    CGE_LOG_ERROR("Init CGEBlendWithSelfFilter failed, Mode number %d\n", mode);
    return false;
}

void CGEBlendVignetteFilter::setVignetteCenter(float x, float y)
{
    m_program.bind();
    m_program.sendUniformf(paramVignetteCenterName, x, y);
}

void CGEBlendVignetteFilter::setVignette(float start, float range)
{
    m_program.bind();
    m_program.sendUniformf(paramVignetteName, start, range);
}

//////////////////////////////////////////////////////////////////////////

bool CGEBlendVignette2Filter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendVignette2HelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        setVignetteCenter(0.5f, 0.5f);
        return true;
    }
    CGE_LOG_ERROR("Init CGEBlendWithSelfFilter failed, Mode number %d\n", mode);
    return false;
}

bool CGEBlendVignetteNoAlphaFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendVignetteNoAlphaHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        setVignetteCenter(0.5f, 0.5f);
        return true;
    }
    CGE_LOG_ERROR("Init CGEBlendWithSelfFilter failed, Mode number %d\n", mode);
    return false;
}

bool CGEBlendVignette2NoAlphaFilter::initWithMode(CGETextureBlendMode mode)
{
    char buffer[BUFFER_LEN];
    if (mode < 0 || mode >= s_nShaderEnum) return false;
    sprintf(buffer, s_blendVignette2NoAlphaHelpStart, s_shaderEnum[mode]);
    if (initShadersFromString(g_vshDefaultWithoutTexCoord, buffer))
    {
        setVignetteCenter(0.5f, 0.5f);
        return true;
    }
    CGE_LOG_ERROR("Init CGEBlendWithSelfFilter failed, Mode number %d\n", mode);
    return false;
}
} // namespace CGE
