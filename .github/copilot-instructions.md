# GitHub Copilot Instructions — android-gpuimage-plus

This repo is a widely-used Android GPU filter library. Prefer **small, safe, backward-compatible** changes.

## Non‑negotiables (stability first)

- **Do not break compatibility**: avoid changing public Java APIs, JNI method signatures, or filter rule string syntax.
- Prefer **additive** changes (new classes/methods/filters) over refactors.
- Keep FFmpeg/video code **fully optional** (both Java and native side guards).

## Where things live (only what’s non-obvious)

- Java public API / JNI front door: `library/src/main/java/org/wysaid/nativePort/`
  - `CGENativeLibrary`, `CGEImageHandler`, `CGEFrameRenderer`, `CGEFrameRecorder`
- Native engine: `library/src/main/jni/cge/`
- JNI bridge (Java ↔ C++): `library/src/main/jni/interface/`
- Extension filters (built as `libCGEExt.so`): `library/src/main/jni/custom/`
- Prebuilt FFmpeg variants: `library/src/main/jni/ffmpeg/` and `ffmpeg-16kb/`
- Demo app (`cgeDemo/`) is for examples; avoid coupling new library APIs to demo-only code.

## Build toggles you must respect

These flags come from `local.properties` (see `settings.gradle`):

- `usingCMakeCompile`: build native code via CMake; otherwise use prebuilt `.so` in `library/src/main/libs/`.
- `usingCMakeCompileDebug`: toggles Debug vs Release native build.
- `disableVideoModule`: when true, **no FFmpeg / no recording**.
- `enable16kPageSizes`: enables 16KB page-size linking flags and uses `ffmpeg-16kb`.
- `deployArtifacts`: enables maven publishing tasks.

Important: CMake uses recursive globs, but **ndk-build (`Android.mk`) lists sources explicitly**. If you add native files that must compile in ndk-build mode, update `Android.mk` accordingly.

## Native/C++ conventions that matter here

- Follow `.clang-format` (Allman braces, 4 spaces, no tabs). Keep code in `namespace CGE {}`.
- **No C++ exceptions** (compiled with `-fno-exceptions`). Use return values and log errors.
- Use project logging macros: `CGE_LOG_INFO`, `CGE_LOG_KEEP`, `CGE_LOG_ERROR`.
- Inline shader strings via `CGE_SHADER_STRING_PRECISION_H()` / `CGE_SHADER_STRING_PRECISION_M()`.

## Java/OpenGL thread rules (project-specific pitfalls)

- For `GLSurfaceView`-based classes, do GL operations on the GL thread via `queueEvent(...)`.
- `NativeLibraryLoader` controls native library loading order; don’t unconditionally load FFmpeg when video module is disabled.

## Adding a new GPU filter / feature (minimal checklist)

1. Implement the filter in `library/src/main/jni/custom/` (or `cge/filters/` if it’s core).
2. Initialize shaders/uniforms in `init()`; handle compile/link failures without crashing.
3. Ensure GL resources are released (programs/textures/FBOs) along the existing lifecycle.
4. If the feature is exposed to Java, add/update JNI wrappers under `library/src/main/jni/interface/` and keep signatures stable.
5. If it should be reachable from rule strings, update the parsing/registration in the native engine (don’t change existing syntax).
6. Keep FFmpeg/video-dependent logic behind `disableVideoModule` / `CGE_USE_VIDEO_MODULE` guards.

## Communication & docs

- Use **English** for code comments, commit messages, and PR descriptions.
- Don’t add new “how to build” tutorials here—put long-form docs in `README.md` (this file should stay short).
