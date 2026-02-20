---
description: "Code conventions for C++, Java, and JNI bridge layers. Covers naming, safety patterns, and resource management."
applyTo: "library/src/main/jni/**/*.cpp,library/src/main/jni/**/*.h,library/src/main/jni/**/*.c,library/src/main/java/**/*.java,cgeDemo/src/main/java/**/*.java"
---

# C++ / Native

- All code in `namespace CGE {}`.
- No C++ exceptions (`-fno-exceptions`). Return error codes; never `throw`.
- Logging: `CGE_LOG_INFO` / `CGE_LOG_ERROR` / `CGE_LOG_KEEP` — never raw `__android_log_print`.
- Inline GLSL via `CGE_SHADER_STRING_PRECISION_H()` / `CGE_SHADER_STRING_PRECISION_M()`.
- GL resources (programs, textures, FBOs) must be released along the handler lifecycle.

# JNI Bridge (`interface/`)

- JNI function names follow `Java_org_wysaid_nativePort_<Class>_<method>`.
- Respect `NativeLibraryLoader`'s library loading order.

# Java / Android

- Native methods are loaded through `NativeLibraryLoader` — don't add direct `System.loadLibrary` calls.
