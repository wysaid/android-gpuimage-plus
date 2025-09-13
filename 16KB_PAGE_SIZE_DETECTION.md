# 16KB Page Size Detection in CGE Library

This document describes the 16KB page size runtime detection feature added to the CGE (Computer Graphics Engine) library.

## Overview

The CGE library now automatically detects and reports the status of 16KB page size support when the native library is loaded. This helps developers understand whether their app is properly configured for 16KB page sizes, which are required by Google for Android apps.

## How It Works

The detection runs automatically in the `JNI_OnLoad` function when the CGE native library is loaded. It performs three levels of checks:

### 1. Compile-time Check
- Verifies if `ENABLE_16K_PAGE_SIZES` was defined during compilation
- Controlled by the `gradle.ext.enable16kPageSizes` build property

### 2. Runtime System Check
- Checks if the Android device supports 16KB page sizes
- Requires Android API level 35+ (Android 15+)

### 3. Active Usage Check
- Verifies if 16KB page sizes are actually being used
- Uses `sysconf(_SC_PAGESIZE)` to check current page size

## Status Reports

The detection reports one of four possible states:

### `DISABLED`
- **Meaning**: 16KB page size support was not enabled at compile time
- **Action**: Enable 16KB support in build configuration

### `COMPILE_READY` 
- **Meaning**: 16KB support is compiled in, but Android version is too old
- **Details**: Device API level < 35 (Android 15)
- **Action**: Feature will work on newer Android versions

### `SUPPORTED_BUT_INACTIVE`
- **Meaning**: All support is available but 16KB pages are not currently active
- **Details**: System supports it but current page size ≠ 16KB
- **Action**: May indicate system configuration or other factors

### `ACTIVE`
- **Meaning**: 16KB page size is fully working
- **Details**: All checks pass and current page size is 16KB
- **Action**: Success! 16KB page sizes are active

## Log Output Example

When the CGE library loads, you'll see logs like this:

```
I/libCGE: === CGE 16KB Page Size Status ===
I/libCGE: Status: ACTIVE
I/libCGE: Details: 16KB page size is fully active and working
I/libCGE: Compile-time support: YES
I/libCGE: Android API Level: 35 (16KB support needs API 35+)
I/libCGE: Current page size: 16384 bytes
I/libCGE: 16KB pages active: YES
I/libCGE: ================================
```

## Configuration

To enable 16KB page size support in your build:

### gradle.properties
```properties
gradle.ext.enable16kPageSizes = true
gradle.ext.usingCMakeCompile = true
```

### Alternative: Command Line
```bash
./gradlew assembleDebug -Pgradle.ext.enable16kPageSizes=true -Pgradle.ext.usingCMakeCompile=true
```

## Implementation Details

- **File**: `library/src/main/jni/interface/cgeNativeLibrary.cpp`
- **Function**: `cgeCheck16kPageSize()`
- **Trigger**: Called in `JNI_OnLoad()` during library initialization
- **Dependencies**: 
  - `unistd.h` for `sysconf()`
  - `android/api-level.h` for `android_get_device_api_level()`

## Benefits

1. **Debugging**: Quickly identify 16KB page size configuration issues
2. **Verification**: Confirm that 16KB support is working as expected
3. **Monitoring**: Track 16KB page size status across different devices
4. **Compliance**: Help ensure Google Play Store requirements are met

## Notes

- The detection runs automatically when CGE library loads
- No additional API calls required from your application
- Logs use the standard CGE logging system (tag: "libCGE")
- Works with both debug and release builds (when logging is enabled)