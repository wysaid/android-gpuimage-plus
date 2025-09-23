# Camera2 API Integration

## Overview

This document describes the Camera2 API integration that has been added to the android-gpuimage-plus library. The implementation provides a flexible camera backend system that supports both the legacy Camera API and the modern Camera2 API.

## Features

- **Backward Compatibility**: Existing code continues to work with the legacy Camera API by default
- **Camera2 Support**: Full Camera2 API support for devices with API level 21+
- **Runtime Switching**: Users can choose which camera backend to use through UI controls
- **Automatic Fallback**: Graceful fallback to legacy API when Camera2 is not available
- **Persistent Settings**: User camera backend preferences are saved and restored

## Architecture

### Core Components

1. **CameraBackend** - Abstract base class defining the camera interface
2. **CameraBackendLegacy** - Implementation wrapping the legacy Camera API
3. **CameraBackend2** - Implementation using the Camera2 API
4. **CameraBackendFactory** - Factory class for creating camera backends
5. **CameraInstance** - Modified singleton that now delegates to backends

### Backend Selection

The camera backend can be selected using:

```java
// Set Camera2 backend
CameraInstance.setCameraBackendType(CameraBackendFactory.CameraBackendType.CAMERA2);

// Set legacy backend
CameraInstance.setCameraBackendType(CameraBackendFactory.CameraBackendType.LEGACY);

// Automatic selection (Camera2 if available, otherwise legacy)
CameraInstance.setCameraBackendType(CameraBackendFactory.CameraBackendType.AUTO);
```

## Usage

### For Library Users

The library remains fully backward compatible. No changes are required for existing code.

To enable Camera2:

```java
// Initialize with context (required for Camera2)
CameraInstance.getInstance().initializeWithContext(context);

// Enable Camera2 backend
CameraInstance.setCameraBackendType(CameraBackendFactory.CameraBackendType.CAMERA2);

// Use camera normally
CameraInstance.getInstance().tryOpenCamera(callback);
```

### For Demo Application

The demo application now includes a configuration panel in MainActivity:

1. **Checkbox**: "Use Camera2 API" - toggles between Camera backends
2. **Status Display**: Shows current backend status and device compatibility
3. **Info Button**: Displays detailed runtime information

Settings are automatically saved and restored between app sessions.

## Implementation Details

### CameraBackend Interface

The `CameraBackend` abstract class defines a unified interface for camera operations:

- `tryOpenCamera()` - Open camera with specified facing
- `stopCamera()` - Close camera and cleanup
- `startPreview()` / `stopPreview()` - Control preview
- `setPictureSize()` / `setFocusMode()` - Configuration
- `focusAtPoint()` - Touch-to-focus functionality

### Legacy Compatibility

The `CameraBackendLegacy` class wraps the existing Camera API implementation, ensuring:

- All existing functionality is preserved
- No breaking changes to the API
- Same behavior as before the refactoring

### Camera2 Implementation

The `CameraBackend2` class provides:

- Modern Camera2 API usage with CameraDevice and CameraCaptureSession
- Background thread handling for camera operations
- Proper lifecycle management
- Permission checking
- Size selection and configuration

### Context Initialization

Camera2 requires a Context for CameraManager access. The library now supports context initialization:

```java
// Initialize once, typically in Application or Activity
CameraInstance.getInstance().initializeWithContext(context);
```

This is automatically handled in CameraGLSurfaceView constructor.

## Error Handling

The implementation includes comprehensive error handling:

- **Permission Errors**: Camera2 checks for camera permissions
- **Device Compatibility**: Automatic fallback when Camera2 is not supported
- **Camera Access Errors**: Proper cleanup on camera open failures
- **Lifecycle Errors**: Safe handling of Activity/Context lifecycle

## Testing

Use `CameraBackendTest` class for validation:

```java
// Run comprehensive backend tests
CameraBackendTest.testCameraBackends(context);

// Get runtime information
String info = CameraBackendTest.getRuntimeInfo(context);
```

## Benefits

1. **Future-Proof**: Ready for modern Android camera capabilities
2. **Better Performance**: Camera2 API provides better control and performance
3. **Enhanced Features**: Access to Camera2-specific features like manual controls
4. **Maintained Compatibility**: Existing applications continue to work unchanged
5. **User Choice**: Developers and users can choose the best backend for their needs

## Requirements

- **Minimum SDK**: API 21 (Android 5.0) for Camera2 features
- **Permissions**: CAMERA permission required (same as before)
- **Context**: Application context needed for Camera2 initialization

## Migration Guide

### For Existing Applications

No changes required - applications continue to work with legacy Camera API by default.

### To Enable Camera2

1. Ensure `initializeWithContext()` is called (automatic in CameraGLSurfaceView)
2. Set Camera2 backend type
3. Test on target devices
4. Consider providing user choice in settings

## Future Enhancements

The architecture supports future enhancements:

- Camera2 advanced features (manual controls, RAW capture, etc.)
- CameraX backend implementation
- Device-specific optimizations
- Feature detection and capabilities reporting