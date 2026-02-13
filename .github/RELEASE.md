# Release Process

This project uses an automated release workflow triggered by version tags.

## Creating a Release

### 1. Update the Version

Edit `versionName` in `build.gradle`:

```gradle
ext {
    android = [
        ...
        versionName: "3.1.1",
        ...
    ]
}
```

### 2. Commit and Push

```bash
git add build.gradle
git commit -m "chore: bump version to 3.1.1"
git push
```

### 3. Tag and Push

```bash
git tag v3.1.1
git push origin v3.1.1
```

## Automated Workflow

When a tag matching `v*.*.*` is pushed, the [release workflow](../.github/workflows/release.yml) will:

1. **Validate** that the tag version matches `versionName` in `build.gradle`
2. **Build** four AAR variants:
   - `gpuimage-plus-{version}.aar` — Full with FFmpeg, 4KB page size
   - `gpuimage-plus-{version}-16k.aar` — Full with FFmpeg, 16KB page size
   - `gpuimage-plus-{version}-min.aar` — Image-only, 4KB page size
   - `gpuimage-plus-{version}-16k-min.aar` — Image-only, 16KB page size
3. **Build** the demo APK (with video module)
4. **Create** a GitHub release with all artifacts and release notes

## Tag Format

- Official: `v3.1.1`
- Beta: `v3.1.1-beta1`
- Alpha: `v3.1.1-alpha1`
- Release candidate: `v3.1.1-rc1`

All version parts must be pure numbers. The tag version must exactly match `versionName` in `build.gradle`.
