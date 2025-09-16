
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Conditional 16KB page size support for Android 15+ compliance
- Automatic 16KB page size detection and status reporting
- Support for OpenGL ES 3.0 and 3.1
- Compute shader support
- WaveForm and Histogram filters
- CMake build system support
- VSCode build tasks and integration
- Android CI workflow
- Matrix control demo for FilterView

### Changed
- Replaced GL_TRIANGLE_FAN with GL_TRIANGLE_STRIP for better performance
- Upgraded to Android Gradle Plugin (AGP) latest version
- Improved build system with better NDK path detection
- Enhanced Maven publishing with local repository support
- Better file structure organization
- Improved Mali GPU compatibility

### Fixed
- Fixed GL stub symbol conflicts for 16KB page size support
- Fixed immutable texture creation issues
- Fixed permission requirements for Android 15+ target SDK
- Fixed android:exported requirement for modern Android versions
- Fixed multiple camera support issues
- Fixed crashes on low-end devices
- Fixed assert not found errors
- Fixed LUT intensity settings bugs

### Removed
- OpenCV and FaceTracker dependencies (moved to legacy branch)
- Deprecated files and unused implementations

## [2.7.0-alpha] - 2023-02-06

### Added
- Pre-release with NDK23 support
- Local Maven repository publishing
- Optional video module disable feature
- MinGW build support for Windows
- Mali GPU compatibility improvements

### Changed
- Updated to NDK23
- Better project structure and organization
- Improved gradle dependencies with GitHub hosted Maven repo

### Fixed
- Crash fixes for various device compatibility issues
- Mali GPU rendering issues
- Low-end device performance problems

## [2.6.0] - 2022-04-25

### Added
- LUT intensity settings support
- x86_64 architecture support

### Changed
- Updated FFmpeg to version 3.4.8
- Improved build scripts

### Fixed
- LUT intensity calculation bugs
- Various compilation issues

## [2.5.1] - 2020-09-24

### Fixed
- Multiple camera support issues on devices with telephoto/wide-angle lenses
- Image distortion problems

## [2.5.0] - 2018-06

### Added
- `CameraGLSurfaceViewWithBuffer` class for camera preview with buffer
- Improved face detection demo

### Removed
- Various unused functions

### Fixed
- Face detection demo bugs

## [2.4.6] - 2017-07

### Added
- Android demo applications
- JNI wrappers for native code
- Color mapping filters
- Real-time camera filters (preview, picture, video recording)
- Real-time video filters (preview and save)
- Face landmark module using FaceTracker (optional)

### Fixed
- Various minor bug fixes

---

## Legacy Versions (2014-2015)

<details>
<summary>Click to expand legacy changelog</summary>

### [1.13.1] - 2015-05-13
- Fixed definition of `CGE_CURVE_PRECISION`

### [1.13.0] - 2015-05-11
- Added monochrome filter with Photoshop-compatible color arguments

### [1.12.0] - 2015-04-10
- Added font rendering module with FreeType support

### [1.11.1] - 2015-03-26
- Replaced `glGetAttribLocation` with `glBindAttribLocation`

### [1.11.0] - 2015-03-26
- Fixed invalid usage of glPixelStorei

### [1.10.1] - 2015-02-04
- Fixed scene roaming calculation errors

### [1.10.0] - 2015-01-26
- Added support for all Photoshop blend modes
- Optimized shader random function for better blur effects
- Added fast filter judgment in cgeInitialize
- Completed slideshow3d with scene roaming support

### [1.9.1] - 2015-01-13
- Optimized slideshow code structure

### [1.9.0] - 2015-01-08
- Added 3D sprite support
- Added timeline action support for scene control

### [1.8.1] - 2014-12-18
- Improved iOS compatibility
- Reduced library size by removing inline functions

### [1.8.0] - 2014-12-16
- Added alpha channel support for images
- Fixed iOS device compatibility issues

### [1.7.1] - 2014-12-16
- Fixed curve texture creation on OpenGL ES 2.0 devices

### [1.7.0] - 2014-12-10
- Improved compatibility with older CGE app versions

### [1.6.1] - 2014-11-24
- Fixed vigblend rule parsing

### [1.6.0] - 2014-11-18
- Merged CGE Filter module with additional filters and effects

### [1.5.1] - 2014-11-14
- Added Sprite2dWithSpecialAlpha for gradient effects

### [1.5.0] - 2014-11-12
- Fixed curve module and removed unused files

### [1.4.4] - 2014-11-11
- Optimized Curve Filter

### [1.4.3] - 2014-11-05
- Completed filter generator for libCGE

### [1.4.2] - 2014-10-31
- Optimized GeometryLineStrip2d program instantiation

### [1.4.1] - 2014-10-30
- Added sprite module test cases

### [1.4.0] - 2014-10-29
- Added timeline realtime jumping by progress

### [1.3.0] - 2014-10-25
- Added shader cache for sprite module
- Reduced slideshow initialization time

### [1.2.1] - 2014-10-16
- Added global blending mode macro and texture alpha settings

### [1.2.0] - 2014-10-16
- Optimized code include methods for faster compilation

### [1.1.5] - 2014-10-13
- Added antialiasing for line drawing

### [1.1.4] - 2014-10-11
- Optimized Sprite module structure

### [1.1.1] - 2014-09-25
- Added global array buffer cache system
- Required `cgeInitialize` call

### [1.1.0] - 2014-09-24
- Added support for mixed sprite types in timelines

### [1.0.0] - 2014-09-04
- First stable release

</details>