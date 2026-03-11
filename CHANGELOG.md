
# Changelog

All notable changes to **android-gpuimage-plus** are documented here.
Versions follow [Semantic Versioning](https://semver.org/). Entries are ordered newest-first.

---

## [Unreleased] — on `master`, pending next release

---

## [3.2.0] — 2026-03-12

- **feat:** Add `setZoomRatio(float)` API to `ICameraProvider`, `CameraXProvider`, and `Camera1Provider` (#566)
- **feat:** Add `CameraXProvider` — runtime-selectable camera backend with automatic Camera2 / CameraX fallback (#565)
- **ci:** Release workflow overhaul — draft releases, master validation, direct Maven commit, NDK r27c/r29 (#564)

---

## [3.1.2] — 2026-02-21

- **fix:** Remove redundant `-Wl,-z,relro,-z,now` linker flags from CMakeLists.txt to fix `UnsatisfiedLinkError` on 16KB-page-size builds (e.g. Samsung S21+ / Android 14). Android API 23+ enforces full RELRO by default; the explicit flags caused `PT_GNU_RELRO MemSiz` to exceed the mapped RW LOAD segment on 16KB-aligned builds, triggering an `mprotect()` failure with `ENOMEM`. (#563)

---

## [3.1.1] — 2026-02-18

This release is a significant milestone that consolidates ~3 years of development
since the last tagged release (`2.7.0-alpha`).

### New Features

- **feat:** Add WaveForm & Histogram filter support (`CGEWaveFormFilter`) via GLES 3.1 compute shaders (#510, #512)
- **feat:** Add `--disable-video-module` Gradle flag to build an image-only AAR without FFmpeg (`gpuimage-plus-*-min.aar`) (#516)
- **feat:** Add `--publish-all` task to publish all 4 AAR variants (4KB/16KB × full/min) to Maven Local
- **feat:** Add conditional 16KB page size support for Android 15+ compliance (#537, #546)
- **feat:** Add FFmpeg prebuilt libraries compiled with 16KB page-size alignment (#543)
- **feat:** Add demo `TestCaseActivity` that uses a transform matrix to control `FilterView` (#534)
- **feat:** Support GLES 3.0 / 3.1 and compute shaders
- **feat:** Support CMake build (`usingCMakeCompile` flag in `local.properties`)
- **feat:** Support Windows (MinGW) and macOS Arm64 build environments
- **feat:** Support publishing AAR artifacts to custom Maven repository (`maven.wysaid.org`)
- **feat:** Add `--publish` / `--publish-all` helpers in `tasks.sh`
- **feat:** Add VS Code tasks (`tasks.sh`) for build, run, and publish operations

### Bug Fixes

- **fix:** Fix `UnsatisfiedLinkError` caused by GL stub symbol conflicts on 16KB page-size builds (#545)
- **fix:** Fix `immutable texture` error — `glTexImage2D` cannot resize a texture allocated by `glTexStorage2D` (#541, PR #541)
- **fix:** Fix crash on `Camera1` when camera is not available (#524, #526)
- **fix:** Fix Android 15 forced edge-to-edge obscuring UI elements
- **fix:** Fix deprecated storage permissions for Android 13+
- **fix:** Fix video recording quality regression after AGP upgrade (#561)
- **fix:** Fix device orientation / EXIF rotation detection edge cases
- **fix:** Fix `assert` not found compile error on some NDK versions

### Performance

- **perf:** Replace `GL_TRIANGLE_FAN` with `GL_TRIANGLE_STRIP` across all shaders for better GPU performance (#547)
- **perf:** Improve LUT filter intensity blending in native code

### Maintenance

- **chore:** Upgrade Android Gradle Plugin and Gradle wrapper
- **chore:** Convert all comments and documentation to English (#552)
- **chore:** Restructure `.github/` documentation; add Copilot workspace instructions (#559, #560)
- **chore:** Add CI workflows for Android build and lint checks (#557)
- **chore:** Remove OpenCV & FaceTracker (legacy branch preserved at `face_features` tag)
- **chore:** Clang-format entire C++ codebase; refactor file structure

---

## [2.7.0-alpha] — 2023-02-06

Pre-release snapshot using NDK 23. Not a stable release.

- Early prototype of WaveForm / Histogram filter via compute shader
- CMake build support added (experimental)
- Support for publishing to local Maven repo

---

## [2.5.0] — 2018-06

- **feat:** Add `org.wysaid.view.CameraGLSurfaceViewWithBuffer` for camera preview with buffer callback
- **fix:** Remove some redundant internal functions
- **fix:** Bug fixes for face detection demo

---

## [2.4.6] — 2017-07

- **feat:** Add Android demo activities
- **feat:** Add JNI wrappers for native C++ APIs
- **feat:** Add new filters including color mapping (LUT)
- **feat:** Add real-time camera filter (preview, take picture, record video)
- **feat:** Add real-time video filter (preview and save)
- **feat:** Add face landmark module (via [FaceTracker](https://github.com/kylemcdonald/FaceTracker), optional)
- Various bug fixes and stability improvements

---

## [1.13.1] — 2015-05-13

- **fix:** Correct definition of `CGE_CURVE_PRECISION`

---

## [1.13.0] — 2015-05-11

- **feat:** New filter `monochrome` with arguments `red, green, blue, cyan, magenta, yellow` (matches Photoshop behavior)

---

## [1.12.0] — 2015-04-10

- **feat:** Add font render module (using FreeType)

---

## [1.11.1] — 2015-03-26

- **refactor:** Replace `glGetAttribLocation` with `glBindAttribLocation` throughout; `glGetAttribLocation` is no longer used

---

## [1.11.0] — 2015-03-26

- **fix:** Fix invalid usage of `glPixelStorei`

---

## [1.10.1] — 2015-02-04

- **fix:** Fix incorrect value in scene roaming when watch-point + watch-direction equals zero

---

## [1.10.0] — 2015-01-26

- **feat:** Support all Photoshop blend modes (minor differences such as Dissolve may exist)
- **perf:** Optimize random function in shaders (blur quality improved)
- **perf:** Add fast filter judgment in `cgeInitialize`
- **feat:** Finish `slideshow3d`, support scene roaming; support mixing 2D & 3D sprites in one timeline
- **fix:** Miscellaneous bug fixes

---

## [1.9.1] — 2015-01-13

- **refactor:** Optimize slideshow code structure; reduce unnecessary public functions

---

## [1.9.0] — 2015-01-08

- **feat:** New 3D sprite type
- **feat:** Support adding actions to the timeline (use actions to drive scene changes)
- **refactor:** Optimize overall code structure

---

## [1.8.1] — 2014-12-18

- **fix:** Improve iOS compatibility
- **perf:** Remove some inline functions to reduce library binary size

---

## [1.8.0] — 2014-12-16

- **feat:** Support images with alpha channels — alpha channel is preserved in output
- **fix:** Fix invalid `cvlomo` effect on iOS devices
- **fix:** Various Android compatibility fixes

---

## [1.7.1] — 2014-12-16

- **fix:** Fix curve texture creation failure on some OpenGL ES 2.0 devices

---

## [1.7.0] — 2014-12-10

- **compat:** Maintain backward compatibility with older CGE app versions

---

## [1.6.1] — 2014-11-24

- **fix:** Fix parsing of filter rule `vigblend`

---

## [1.6.0] — 2014-11-18

- **feat:** Merge CGE Filter module; add new filters and effects

---

## [1.5.1] — 2014-11-14

- **feat:** Add `Sprite2dWithSpecialAlpha` for special alpha gradient rendering

---

## [1.5.0] — 2014-11-12

- **fix:** Fix curve module; remove unused files

---

## [1.4.4] — 2014-11-11

- **fix:** Bug fix and optimization for Curve Filter

---

## [1.4.3] — 2014-11-05

- **feat:** Finish filter generator for libCGE

---

## [1.4.2] — 2014-10-31

- **perf:** Make `GeometryLineStrip2d`'s shader program a single instance; avoid recreating it on each use

---

## [1.4.1] — 2014-10-30

- **test:** Add test cases for sprite module

---

## [1.4.0] — 2014-10-29

- **feat:** Timeline real-time seeking by progress value

---

## [1.3.0] — 2014-10-25

- **perf:** Add shader cache for sprite module; reduce slideshow initialization time

---

## [1.2.1] — 2014-10-16

- **feat:** Add global blending mode macro; support global premultiply-or-not setting for texture alpha

---

## [1.2.0] — 2014-10-16

- **perf:** Optimize code inclusion strategy; reduce compile time for slideshow module

---

## [1.1.5] — 2014-10-13

- **feat:** Add anti-alias for line drawing

---

## [1.1.4] — 2014-10-11

- **refactor:** Optimize Sprite module structure

---

## [1.1.1] — 2014-09-25

- **perf:** Use global array buffer cache; avoid re-creating buffers for every instance
- **note:** Requires calling `cgeInitialize` before use

---

## [1.1.0] — 2014-09-24

- **feat:** Support mixed sprite types (different types in the same timeline)

---

## [1.0.0] — 2014-09-04

First stable release. Minor bugs may remain.