---
description: "Use when editing Java Android library or demo code. Covers GL thread safety and native interop patterns."
applyTo: "library/src/main/java/**/*.java,cgeDemo/src/main/java/**/*.java"
---

# Java / Android Layer

- GL operations in `GLSurfaceView` subclasses **must** run on the GL thread via `queueEvent(...)`.
- Native methods are declared `native` and loaded through `NativeLibraryLoader` — don't add direct `System.loadLibrary` calls.
- Don't unconditionally reference FFmpeg classes; check `BuildConfig.CGE_USE_VIDEO_MODULE` first.
- Public API in `org.wysaid.nativePort.*` is consumed by external users — **do not change signatures**.
