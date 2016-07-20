# Android-GPUImage-plus
GPU accelerated filters for Android based on OpenGL. 

## note ##

update 2016-7-7: 
    The video-record feature is available now. <br>
    The LUT filter is supported now.<br>
    New function "@beautify face ..." for face beautification, see the description string below.<br>
    <b>If you just need some image filters, please check the 'no_ffmpeg' branch. It's kept without additional functions</b>

## 简介 ##

1.  This repo is an "Android Studio Project", comprising "cgeDemo", "library" two sub-modules. All java code and the "libCGE.so"(Written in C++&OpenGL with NDK) is provided. Hundreds of built-in filters are available in the demo. 😋If you'd like to add your own filter, please refer to the document for the "Effect String Definition Rule" below.
(本repo为一个Android Studio Project, 包含 cgeDemo, library 两个子模块. 其中library 模块包含java部分所有代码以及一个包含cge核心模块的so链接库，内置近百种滤镜效果， 😋如果要自行添加滤镜， 请参考下方的滤镜描述文件。)

2. Demo and Library will be updated as needed. Welcome for your questions or PR.
(不定期更新demo和library. 如有问题欢迎提问， 也欢迎PR.)

3. For study only, and no free tech support by now.

4. iOS version: [https://github.com/wysaid/ios-gpuimage-plus](https://github.com/wysaid/ios-gpuimage-plus "http://wysaid.org")

5. Extra functions can be provided to the donors such as 'realtime video recording with gpu filters'. See the precompiled apk about this function: [https://github.com/wysaid/android-gpuimage-plus/tree/master/demoRelease](https://github.com/wysaid/android-gpuimage-plus/tree/master/demoRelease "http://wysaid.org")


## Donate ##

Welcome donations. More work may be paid for the donors' issue as thanks.

Alipay:

![Alipay](https://raw.githubusercontent.com/wysaid/android-gpuimage-plus/master/screenshots/alipay.jpg "alipay")

Paypal: 

[![Paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif "Paypal")](http://blog.wysaid.org/p/donate.html)


## Manual ##

[https://github.com/wysaid/android-gpuimage-plus/wiki/Parsing-String-Rule](https://github.com/wysaid/android-gpuimage-plus/wiki/Parsing-String-Rule "http://wysaid.org")
