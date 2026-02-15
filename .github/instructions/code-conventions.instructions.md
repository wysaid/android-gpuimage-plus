---
description: "Code conventions for C++, Java, and JNI bridge layers. Covers naming, safety patterns, and resource management."
applyTo: "library/src/main/jni/**/*.cpp,library/src/main/jni/**/*.h,library/src/main/jni/**/*.c,library/src/main/java/**/*.java,cgeDemo/src/main/java/**/*.java"
---

# C++ / Native

- Follow `.clang-format` (Allman braces, 4 spaces). All code in `namespace CGE {}`.
- No C++ exceptions (`-fno-exceptions`). Return error codes; never `throw`.
- Logging: `CGE_LOG_INFO` / `CGE_LOG_ERROR` / `CGE_LOG_KEEP` — never raw `__android_log_print`.
- Inline GLSL via `CGE_SHADER_STRING_PRECISION_H()` / `CGE_SHADER_STRING_PRECISION_M()`.
- GL resources (programs, textures, FBOs) must be released along the handler lifecycle; shader compile failures must be logged, not crash.

# JNI Bridge (`interface/`)

- JNI function names follow `Java_org_wysaid_nativePort_<Class>_<method>`.
- Always validate JNI params (null bitmap, null env, invalid handler pointer) before dereferencing.
- **Never** store JNI local refs across calls.
- Respect `NativeLibraryLoader`'s library loading order.
- Any new JNI method needs a corresponding Java `native` declaration in `org.wysaid.nativePort.*`.

# Java / Android

- Native methods are loaded through `NativeLibraryLoader` — don't add direct `System.loadLibrary` calls.
