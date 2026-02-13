---
description: "Use when editing C++ native code. Covers CGE engine conventions, shader patterns, memory safety, and OpenGL ES resource management."
applyTo: "library/src/main/jni/{cge,custom,interface}/**/*.{cpp,h,c}"
---

# C++ / Native Layer

- Follow `.clang-format` (Allman braces, 4 spaces). All code in `namespace CGE {}`.
- No C++ exceptions (`-fno-exceptions`). Return error codes; never `throw`.
- Logging: `CGE_LOG_INFO` / `CGE_LOG_ERROR` / `CGE_LOG_KEEP` — never raw `__android_log_print`.
- Inline GLSL via `CGE_SHADER_STRING_PRECISION_H()` / `CGE_SHADER_STRING_PRECISION_M()`.
- GL resources (programs, textures, FBOs) must be released along the handler lifecycle; shader compile failures must be logged, not crash.
- FFmpeg-dependent code: guard with `#ifdef CGE_USE_VIDEO_MODULE`.
- When adding source files: CMake uses `GLOB_RECURSE` (auto-picks up), but `Android.mk` lists files **explicitly** — update both.
