# Build Guide

## Build Configuration

All build options are controlled via `local.properties` in the project root:

| Property | Default | Effect |
|----------|---------|--------|
| `usingCMakeCompile` | `false` | `true` = build native code with CMake; `false` = use prebuilt `.so` from `library/src/main/libs/` |
| `usingCMakeCompileDebug` | `false` | `true` = Debug native build; `false` = Release |
| `disableVideoModule` | `false` | `true` = exclude FFmpeg and video recording features |
| `enable16kPageSizes` | `false` | `true` = enable [16KB page-size](https://developer.android.com/guide/practices/page-sizes#16-kb-impact) linking and use `ffmpeg-16kb` |
| `deployArtifacts` | `false` | `true` = enable Maven publishing tasks |

## Build Methods

### Android Studio (Recommended)

1. Set `usingCMakeCompile=true` in `local.properties`
2. Open the project in Android Studio (latest version)
3. Wait for NDK/CMake initialization
4. Build and run

### Visual Studio Code

> Requires [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) (recommended), [MinGW](https://osdn.net/projects/mingw/), or [Cygwin](https://www.cygwin.com/) on Windows.

1. Set environment variable `ANDROID_HOME` to your Android SDK directory
2. Open the project in VS Code
3. Press `⌘ + Shift + B` (Mac) or `Ctrl + Shift + B` (Win/Linux)
4. Choose "Enable CMake And Build Project With CMake"

### Command Line (tasks.sh)

```bash
# Set Android SDK path (Linux/Mac)
export ANDROID_HOME=/path/to/android/sdk

# Setup project
bash tasks.sh --setup-project

# Build (Debug, CMake)
bash tasks.sh --debug --enable-cmake --build

# Build (Release, CMake)
bash tasks.sh --release --enable-cmake --build

# Run demo
bash tasks.sh --run

# Build without video module
bash tasks.sh --enable-cmake --disable-video-module --build

# Enable 16KB page size
bash tasks.sh --enable-cmake --enable-16kb-page-size --build
```

Run `bash tasks.sh --help` for all available options.

### ndk-build (Legacy)

> **Not recommended.** Use CMake instead. This method is kept for backward compatibility.

```bash
export NDK=/path/to/ndk
cd library/src/main/jni

# Build all architectures with video module
export CGE_USE_VIDEO_MODULE=1
./buildJNI

# Build without video module
unset CGE_USE_VIDEO_MODULE
$NDK/ndk-build

# Enable 16KB page sizes (optional)
export CGE_ENABLE_16KB_PAGE_SIZE=1
./buildJNI
```

> Precompiled libs (NDK-r23b): [android-gpuimage-plus-libs](https://github.com/wysaid/android-gpuimage-plus-libs)

## Dual Build System

This project supports both CMake and ndk-build:

- **CMake** (`CMakeLists.txt`): Uses `GLOB_RECURSE` — new source files are picked up automatically.
- **ndk-build** (`Android.mk`): Lists source files **explicitly** — you must update it manually when adding files.

When adding new native source files, always ensure both build systems are updated.

## Publish AAR to Maven Local

```bash
# Full-featured
bash tasks.sh --enable-cmake --enable-video-module --publish

# Image-only (no FFmpeg)
bash tasks.sh --enable-cmake --disable-video-module --publish
```

This publishes to the local Maven repository configured by `localRepoDir` in `local.properties`.
