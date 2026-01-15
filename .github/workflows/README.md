# CI/CD Workflow 说明

## 概述

本项目使用 GitHub Actions 进行持续集成和构建。Workflow 配置文件：

- [ubuntu-build.yml](./ubuntu-build.yml)
- [windows-build.yml](./windows-build.yml)
- [macos-build.yml](./macos-build.yml)

发布流程配置文件：[release.yml](./release.yml)

## 构建矩阵

### 维度说明

项目支持以下构建配置的组合：

1. **操作系统** (3种)
   - Windows (windows-latest)
   - macOS (macos-latest)
   - Ubuntu (ubuntu-latest)

2. **NDK 版本** (2种)
   - r26d (稳定版本)
   - r27c (较新版本)

3. **构建系统** (2种)
   - CMake (`--enable-cmake`)
   - ndk-build (传统构建方式)

4. **FFmpeg 支持** (2种)
   - with-ffmpeg (`--enable-video-module`) - 支持视频录制功能
   - no-ffmpeg (`--disable-video-module`) - 纯图片处理，体积更小

5. **内存页大小** (2种)
   - 4kb (默认) (`--disable-16kb-page-size`)
   - 16kb (`--enable-16kb-page-size`) - 针对新 Android 设备优化

理论上共有 **3 × 2 × 2 × 2 × 2 = 48** 种组合。

## 触发策略

### 1. Master 分支推送 (全量构建)

当代码推送到 `master` 分支时，运行**所有 48 个构建任务**，确保所有配置组合都能正常工作。

```yaml
on:
  push:
    branches: [ "master" ]
```

**构建任务数量：** 48
- Windows: 16 (2 NDK × 2 build-system × 2 ffmpeg × 2 page-size)
- macOS: 16 (2 NDK × 2 build-system × 2 ffmpeg × 2 page-size)
- Ubuntu: 16 (2 NDK × 2 build-system × 2 ffmpeg × 2 page-size)

### 2. Pull Request (精简构建)

PR 时运行**精简的 18 个构建任务**，在保证质量的同时节省资源：

```yaml
on:
  pull_request:
    branches: [ "master" ]
```

**构建任务数量：** 18

#### Windows (1个任务)
- NDK: r26d
- 构建系统: CMake
- FFmpeg: 启用
- 页大小: 16KB

#### macOS (1个任务)
- NDK: r27c
- 构建系统: ndk-build
- FFmpeg: 禁用
- 页大小: 4KB

#### Ubuntu (16个任务 - 全量)
- NDK: r26d + r27c
- 构建系统: CMake + ndk-build
- FFmpeg: 启用 + 禁用
- 页大小: 4KB + 16KB
- 全部组合: 2 × 2 × 2 × 2 = 16

### 3. 其他分支推送

非 master 分支的推送**不会触发** workflow，避免资源浪费。只有创建 PR 时才会触发精简构建。

## 产物命名规则

构建产物 (APK) 使用以下命名格式：

```
apk-{OS}-{NDK}-{BuildSystem}-{FFmpeg}-{PageSize}.apk
```

**示例：**
- `apk-Linux-r26d-cmake-ffmpeg-4kb.apk`
- `apk-Windows-r27c-ndk-build-noffmpeg-16kb.apk`
- `apk-macOS-r26d-cmake-ffmpeg-16kb.apk`

## Lint 检查

代码质量检查仅在 master 分支推送时运行，确保主干代码质量。

## 本地测试

可以使用项目根目录的 `tasks.sh` 脚本在本地进行构建测试：

```bash
# CMake 构建，启用视频模块，16KB 页大小
./tasks.sh --release --enable-cmake --enable-video-module --enable-16kb-page-size --build

# ndk-build 构建，禁用视频模块，默认页大小
./tasks.sh --release --disable-video-module --disable-16kb-page-size --build
```

## 注意事项

1. **NDK 版本**：确保本地安装了对应版本的 NDK
2. **构建时间**：全量构建（48个任务）可能需要较长时间
3. **产物大小**：启用 FFmpeg 的版本体积较大（包含视频编解码库）
4. **页大小**：16KB 页大小版本需要 Android API 35+ 支持

## 资源优化

通过 PR 精简构建策略（18个任务 vs 48个任务），可以：
- 节省约 62.5% 的 CI/CD 资源
- 缩短 PR 反馈时间
- 保持足够的测试覆盖率（Ubuntu 全量测试）
