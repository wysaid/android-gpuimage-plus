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

* Important options in `local.properties`:
  * `usingCMakeCompile=true`: Set to true to compile the native library with CMake. Defaults to false, allowing the use of prebuilt libraries.
  * `usingCMakeCompileDebug=true`: Set to true to compile the native library in Debug Mode. Defaults to false.
  * `disableVideoModule=true`: Set to true to disable the video recording feature, which is useful for image-only scenarios. This reduces the size of the native library significantly. Defaults to false.
  * `enable16kPageSizes=true`: Set to true to enable [16k page sizes](https://developer.android.com/guide/practices/page-sizes#16-kb-impact) for the native library, applicable only in CMake compile mode. Defaults to false.

* Build with `Android Studio` and CMake: (Recommended)
  * Put `usingCMakeCompile=true` in your `local.properties`
  * Open the repo with the latest version of `Android Studio`
  * Waiting for the initialization. (NDK/cmake install)
  * Done.

* Using `Visual Studio Code`: (Requires __[WSL(Recommended)](https://learn.microsoft.com/en-us/windows/wsl/install)__/__[MinGW](https://osdn.net/projects/mingw/)__/__[Cygwin](https://www.cygwin.com/)__ on Windows.)
  * Setup ENV variable `ANDROID_HOME` to your Android SDK installation directory.
  * Open the repo with `Visual Studio Code`
  * Press `⌘ + shift + B` (Mac) or `ctrl + shift + B` (Win/Linux), choose the option `Enable CMake And Build Project With CMake`.
  * Done.

* Build with preset tasks: (Requires __[WSL(Recommended)](https://learn.microsoft.com/en-us/windows/wsl/install)__/__[MinGW](https://osdn.net/projects/mingw/)__/__[Cygwin](https://www.cygwin.com/)__ on Windows.)

  ```shell
  # define the environment variable "ANDROID_HOME"
  # If using Windows, define ANDROID_HOME in Windows Environment Settings by yourself.
  export ANDROID_HOME=/path/to/android/sdk
  
  # Setup Project
  bash tasks.sh --setup-project

  # Compile with CMake Debug
  bash tasks.sh --debug --enable-cmake --build
  # Compile with CMake Release
  bash tasks.sh --release --enable-cmake --build

  # Start Demo By Command
  bash tasks.sh --run
  ```

* Build `JNI` part with ndk-build: (Not recommended)

  ```shell
  export NDK=path/of/your/ndk
  cd folder/of/jni (android-gpuimage-plus/library/src/main/jni)
  
  # Enable 16kb page sizes (optional)
  export CGE_ENABLE_16KB_PAGE_SIZE=1

  #This will make all arch: armeabi, armeabi-v7a arm64-v8a, x86, mips
  ./buildJNI
  #Or use "sh buildJNI"
  
  #Try this if you failed to run the shell above
  export CGE_USE_VIDEO_MODULE=1
  $NDK/ndk-build
  
  #If you don't want anything except the image filter,
  #Do as below to build with only cge module
  #No ffmpeg, opencv or faceTracker.
  #And remove the loading part of ffmpeg&facetracker
  $NDK/ndk-build
  
  #For Windows user, you should include the `.cmd` extension to `ndk-build` like this:
  cd <your\path\to\this\repo>\library\src\main\jni
  <your\path\to\ndk>\ndk-build.cmd
  
  #Also remember to comment out these line in NativeLibraryLoader
  //System.loadLibrary("ffmpeg");
  //CGEFFmpegNativeLibrary.avRegisterAll();
  ```

> You can find precompiled libs here: [android-gpuimage-plus-libs](https://github.com/wysaid/android-gpuimage-plus-libs) (The precompiled '.so' files are generated with NDK-r23b)

Note that the generated file "libFaceTracker.so" is not necessary. So just remove this file if you don't want any feature of it.

* iOS version: [https://github.com/wysaid/ios-gpuimage-plus](https://github.com/wysaid/ios-gpuimage-plus "http://wysaid.org")

## Manual

### 1. Usage

___Sample Code for doing a filter with Bitmap___

```java
//Simply apply a filter to a Bitmap.
@Override
protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    Bitmap srcImage = ...;

    //HSL Adjust (hue: 0.02, saturation: -0.31, luminance: -0.17)
    //Please see the manual for more details.
    String ruleString = "@adjust hsl 0.02 -0.31 -0.17";

    Bitmap dstImage = CGENativeLibrary.filterImage_MultipleEffects(src, ruleString, 1.0f);

    //Then the dstImage is applied with the filter.

    //Save the result image to /sdcard/libCGE/rec_???.jpg.
    ImageUtil.saveBitmap(dstImage);
}
```

### 2. Custom Shader Filter

#### 2.1 Write your own filter

>Your filter must inherit [CGEImageFilterInterfaceAbstract](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/jni/include/cgeImageFilter.h#L42) or its child class. Most of the filters are inherited from [CGEImageFilterInterface](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/jni/include/cgeImageFilter.h#L57) because it has many useful functions.

```cpp
// A simple customized filter to do a color reversal.
class MyCustomFilter : public CGE::CGEImageFilterInterface
{
public:
    
    bool init()
    {
        CGEConstString fragmentShaderString = CGE_SHADER_STRING_PRECISION_H
        (
        varying vec2 textureCoordinate;  //defined in 'g_vshDefaultWithoutTexCoord'
        uniform sampler2D inputImageTexture; // the same to above.

        void main()
        {
            vec4 src = texture2D(inputImageTexture, textureCoordinate);
            src.rgb = 1.0 - src.rgb;  //Simply reverse all channels.
            gl_FragColor = src;
        }
        );

        //m_program is defined in 'CGEImageFilterInterface'
        return m_program.initWithShaderStrings(g_vshDefaultWithoutTexCoord, s_fsh);
    }

    //void render2Texture(CGE::CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID)
    //{
    //  //Your own render functions here.
    //  //Do not override this function to use the CGEImageFilterInterface's.
    //}
};
```

>Note: To add your own shader filter with c++. [Please see the demo for further details](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/jni/source/customFilter_N.cpp).

#### 2.2 Run your own filter

__In C++, you can use a CGEImageHandler to do that:__

```cpp
//Assume the gl context already exists:
//JNIEnv* env = ...;
//jobject bitmap = ...;
CGEImageHandlerAndroid handler;
CustomFilterType* customFilter = new CustomFilterType();

//You should handle the return value (false is returned when failed.)
customFilter->init();
handler.initWithBitmap(env, bitmap);

//The customFilter will be released when the handler' destructor is called.
//So you don't have to call 'delete customFilter' if you add it into the handler.
handler.addImageFilter(customFilter);

handler.processingFilters(); //Run the filters.

jobject resultBitmap = handler.getResultBitmap(env);
```

>If no gl context exists, the class [CGESharedGLContext](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/jni/interface/cgeSharedGLContext.h#L22) may be helpful.

__In Java, you can simply follow the sample:__

See: [CGENativeLibrary.cgeFilterImageWithCustomFilter](https://github.com/wysaid/android-gpuimage-plus/blob/master/cgeDemo/src/main/java/org/wysaid/cgeDemo/TestCaseActivity.java#L123)

__Or to do with a [CGEImageHandler](https://github.com/wysaid/android-gpuimage-plus/blob/master/library/src/main/java/org/wysaid/nativePort/CGEImageHandler.java#L93)__

### 3. Filter Rule String ###

Doc: <https://github.com/wysaid/android-gpuimage-plus/wiki>

En: [https://github.com/wysaid/android-gpuimage-plus/wiki/Parsing-String-Rule-(EN)](https://github.com/wysaid/android-gpuimage-plus/wiki/Parsing-String-Rule-(EN) "http://wysaid.org")

Ch: [https://github.com/wysaid/android-gpuimage-plus/wiki/Parsing-String-Rule-(ZH)](https://github.com/wysaid/android-gpuimage-plus/wiki/Parsing-String-Rule-(ZH) "http://wysaid.org")

#### Common Filter Examples

**Blur Filters:**
- Mosaic Blur: `@blur mosaic <blurPixels>` 
  - Example: `@blur mosaic 10.0` (blurPixels >= 1.0, default is 1.0 for origin)

**Style Filters:**
- Polar Pixellate: `@style polarpixellate <pixelSizeX>,<pixelSizeY>` or `@style polarpixellate <centerX>,<centerY>,<pixelSizeX>,<pixelSizeY>`
  - Example: `@style polarpixellate 0.05,0.05` (pixel size only, uses default center 0.5,0.5)
  - Example: `@style polarpixellate 0.5,0.5,0.05,0.05` (custom center and pixel size)
  - Center range: [0, 1], Pixel size range: [0, 0.2]

**Dynamic Filters:**
- Motion Flow: `@dynamic mf <totalFrames>,<frameDelay>`
  - Example: `@dynamic mf 10,0` (10 frames with no delay)

## Tool

Some utils are available for creating filters: [https://github.com/wysaid/cge-tools](https://github.com/wysaid/cge-tools "http://wysaid.org")

[![Tool](https://raw.githubusercontent.com/wysaid/cge-tools/master/screenshots/0.jpg "cge-tool")](https://github.com/wysaid/cge-tools)

## License

[MIT License](https://github.com/wysaid/android-gpuimage-plus/blob/master/LICENSE)

## Donate

Alipay:

![Alipay](https://raw.githubusercontent.com/wysaid/android-gpuimage-plus/master/screenshots/alipay.jpg "alipay")

Paypal:

[![Paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif "Paypal")](http://blog.wysaid.org/p/donate.html)
