# Android-GPUImage-Plus

A C++ & Java library for `Image`/`Camera`/`Video` filters. PRs are welcomed.

## New Feature

See the `image deform demo`.

![screenshots](screenshots/6.gif) ![screenshots](screenshots/5.gif)

## Gradle dependency

```gradle
allprojects {
    repositories {
        maven {
            // Use github hosted maven repo for now.
            // Repo url: https://github.com/wysaid/android-gpuimage-plus-maven
            url 'https://maven.wysaid.org/'
        }
    }
}

// Choose only one of them
dependencies {
    // Page size: 4KB (default)
    // Architectures: armeabi-v7a, arm64-v8a, x86, x86_64
    // Full-featured with FFmpeg bundled
    implementation 'org.wysaid:gpuimage-plus:3.1.0'

    // Page size: 16KB
    // Architectures: armeabi-v7a, arm64-v8a, x86, x86_64
    // Full-featured with FFmpeg bundled
    implementation 'org.wysaid:gpuimage-plus:3.1.0-16k'

    // Page size: 4KB (default)
    // Architectures: armeabi-v7a, arm64-v8a, x86, x86_64
    // Image-only version (no video features or FFmpeg)
    implementation 'org.wysaid:gpuimage-plus:3.1.0-min'

    // Page size: 16KB
    // Architectures: armeabi-v7a, arm64-v8a, x86, x86_64
    // Image-only version (no video features or FFmpeg)
    implementation 'org.wysaid:gpuimage-plus:3.1.0-16k-min'
}
```

> To compile other versions of ffmpeg, see: <https://github.com/wysaid/FFmpeg-Android.git>

## Build

Quick start with Android Studio:

1. Add `usingCMakeCompile=true` to `local.properties`
2. Open the project in Android Studio
3. Wait for NDK/CMake initialization
4. Build and run

For detailed build instructions, configuration options, and alternative build methods (VS Code, command line, ndk-build), see [docs/build.md](docs/build.md).

> Precompiled libraries: [android-gpuimage-plus-libs](https://github.com/wysaid/android-gpuimage-plus-libs)  
> iOS version: [ios-gpuimage-plus](https://github.com/wysaid/ios-gpuimage-plus)

## Manual

### 1. Usage

Apply a filter with a rule string:

```java
Bitmap srcImage = ...;

// HSL Adjust (hue: 0.02, saturation: -0.31, luminance: -0.17)
String ruleString = "@adjust hsl 0.02 -0.31 -0.17";

Bitmap dstImage = CGENativeLibrary.filterImage_MultipleEffects(srcImage, ruleString, 1.0f);

// Save result
ImageUtil.saveBitmap(dstImage);
```

### 2. Custom Shader Filter

#### 2.1 Writing a Filter

Custom filters inherit from [`CGEImageFilterInterface`](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/jni/include/cgeImageFilter.h#L57):

```cpp
// A simple color reversal filter
class MyCustomFilter : public CGE::CGEImageFilterInterface
{
public:
    bool init()
    {
        CGEConstString fragmentShaderString = CGE_SHADER_STRING_PRECISION_H
        (
        varying vec2 textureCoordinate;
        uniform sampler2D inputImageTexture;

        void main()
        {
            vec4 src = texture2D(inputImageTexture, textureCoordinate);
            src.rgb = 1.0 - src.rgb;  // Reverse all channels
            gl_FragColor = src;
        }
        );

        return m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, fragmentShaderString);
    }
};
```

> See [customFilter_N.cpp](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/jni/source/customFilter_N.cpp) for a complete example.

#### 2.2 Running a Custom Filter

__In C++:__

```cpp
// Assumes GL context exists
CGEImageHandlerAndroid handler;
auto* customFilter = new MyCustomFilter();

customFilter->init();
handler.initWithBitmap(env, bitmap);
handler.addImageFilter(customFilter);  // Handler takes ownership

handler.processingFilters();
jobject resultBitmap = handler.getResultBitmap(env);
```

> If no GL context exists, use [`CGESharedGLContext`](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/jni/interface/cgeSharedGLContext.h#L22).

__In Java:__

Use [`CGENativeLibrary.cgeFilterImageWithCustomFilter`](https://github.com/wysaid/android-gpuimage-plus/blob/master/cgeDemo/src/main/java/org/wysaid/cgeDemo/TestCaseActivity.java#L123) or [`CGEImageHandler`](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/java/org/wysaid/nativePort/CGEImageHandler.java#L93) directly.

### 3. Filter Rule Strings

Filter effects are defined by text-based rule strings. See [docs/filter-rules.md](docs/filter-rules.md) for complete syntax reference.

Quick example:

```java
// Chain multiple filters with @ separator
String filter = "@curve RGB (0,0) (128,150) (255,255) " +
                "@adjust hsl 0.02 -0.31 -0.17 " +
                "@blend overlay texture.jpg 80";
```

### 4. Key Classes & Threading

| Class | Purpose |
|-------|---------|
| `CGENativeLibrary` | Main entry — static filter methods |
| `CGEImageHandler` | Image handler wrapper |
| `CGEFrameRenderer` | Real-time camera/video renderer |
| `CGEFrameRecorder` | Video recording (requires FFmpeg) |

**Important**: GL operations must run on the GL thread:

```java
glSurfaceView.queueEvent(() -> {
    // Your GL operations here
});
```

### Documentation

- [docs/build.md](docs/build.md) — Full build guide and configuration options
- [docs/filter-rules.md](docs/filter-rules.md) — Complete filter syntax reference
- [.github/CONTRIBUTING.md](.github/CONTRIBUTING.md) — Contributing guidelines
- [.github/RELEASE.md](.github/RELEASE.md) — Release process

## Tool

Some utils are available for creating filters: [https://github.com/wysaid/cge-tools](https://github.com/wysaid/cge-tools "http://wysaid.org")

[![Tool](https://raw.githubusercontent.com/wysaid/cge-tools/master/screenshots/0.jpg "cge-tool")](https://github.com/wysaid/cge-tools)

## License

[MIT License](https://github.com/wysaid/android-gpuimage-plus/blob/master/LICENSE)