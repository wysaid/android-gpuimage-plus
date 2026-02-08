---
description: "Use when working on JNI bridge code between Java and C++. Covers JNI safety, signature stability, and the interface/ directory conventions."
applyTo: "library/src/main/jni/interface/**"
---

# JNI Bridge Layer (`interface/`)

- JNI function names follow `Java_org_wysaid_nativePort_<Class>_<method>`.
- Always validate JNI params (null bitmap, null env, invalid handler pointer) before dereferencing.
- C++ side uses `CGEImageHandlerAndroid` — obtain via `reinterpret_cast` from the Java `long` handle; **never** store JNI local refs across calls.
- `NativeLibraryLoader` loads `libCGE.so` → `libCGEExt.so` → optionally `libffmpeg.so`. Respect this ordering.
- Video/recording wrappers (`cgeFrameRecorder*`, `cgeVideoEncoder*`) must compile-guard with `CGE_USE_VIDEO_MODULE`.
- Any new JNI method needs a corresponding Java `native` declaration in `org.wysaid.nativePort.*` — keep signatures stable for binary compatibility.
