# Contributing Guide

## Getting Started

1. Fork the repository
2. Create a feature branch from `master` (`feat/xxx`, `fix/xxx`, `refactor/xxx`)
3. Make your changes
4. Verify the build: `bash tasks.sh --enable-cmake --build`
5. Submit a pull request to `master`

## Code Standards

- Use **English** for all code comments, commit messages, and PR descriptions
- Follow conventional-commit format for commits: `feat:`, `fix:`, `refactor:`, `chore:`
- One logical change per commit

### C++ (Native)

- Follow `.clang-format` (Allman braces, 4-space indent)
- All code in `namespace CGE {}`
- No C++ exceptions (compiled with `-fno-exceptions`)
- Use logging macros: `CGE_LOG_INFO`, `CGE_LOG_ERROR`, `CGE_LOG_KEEP`
- Inline GLSL via `CGE_SHADER_STRING_PRECISION_H()` / `CGE_SHADER_STRING_PRECISION_M()`
- Release GL resources (programs, textures, FBOs) along the handler lifecycle

### Java (Android)

- GL operations in `GLSurfaceView` subclasses must use `queueEvent(...)`
- Load native libraries through `NativeLibraryLoader` only
- Check `BuildConfig.CGE_USE_VIDEO_MODULE` before referencing FFmpeg classes

### JNI Bridge

- JNI function names: `Java_org_wysaid_nativePort_<Class>_<method>`
- Always validate JNI parameters (null checks) before dereferencing
- Never store JNI local references across calls

## Compatibility Rules

- **Do not change** method signatures in `org.wysaid.nativePort.*`
- **Do not change** existing JNI function names
- **Do not change** existing filter rule string syntax
- New methods, filters, and syntax additions are welcome

## FFmpeg / Video Module

All FFmpeg-dependent code must be optional:

- C++: guard with `#ifdef CGE_USE_VIDEO_MODULE`
- Java: check `BuildConfig.CGE_USE_VIDEO_MODULE`
- Verify the project compiles with `--disable-video-module`

## Dual Build System

When adding new native source files under `library/src/main/jni/`:

- **CMake**: auto-discovered via `GLOB_RECURSE` — usually no action needed
- **ndk-build**: update `Android.mk` to explicitly list the new files

## CI

Pull requests trigger CI on three platforms (macOS, Ubuntu, Windows). PRs run a reduced matrix (1 combination per platform); merges to `master` run the full 16-combination matrix.

Ensure all CI checks pass before requesting review.
