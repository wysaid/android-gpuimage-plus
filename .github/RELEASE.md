# Release Process

Automated release triggered by version tags (`v*.*.*`).

## Creating a Release

1. **Update** `versionName` in [build.gradle](../build.gradle) (e.g., `"3.1.1"`)
2. **Commit, tag, and push**:

   ```bash
   git add build.gradle
   git commit -m "chore: bump version to 3.1.1"
   git tag v3.1.1
   git push && git push origin v3.1.1
   ```

**Important**: Tag version must exactly match `versionName` in `build.gradle`.

## Automated Workflow

The [release workflow](../.github/workflows/release.yml) builds and publishes:

| Artifact | FFmpeg | Page Size |
| --- | --- | --- |
| `gpuimage-plus-{version}.aar` | ✅ | 4KB |
| `gpuimage-plus-{version}-16k.aar` | ✅ | 16KB |
| `gpuimage-plus-{version}-min.aar` | ❌ | 4KB |
| `gpuimage-plus-{version}-16k-min.aar` | ❌ | 16KB |

Plus one demo APK with video module enabled.

## Tag Format

| Type | Format | Example |
|------|--------|---------|
| Official | `vX.Y.Z` | `v3.1.1` |
| Beta | `vX.Y.Z-beta#` | `v3.1.1-beta1` |
| Alpha | `vX.Y.Z-alpha#` | `v3.1.1-alpha1` |
| RC | `vX.Y.Z-rc#` | `v3.1.1-rc1` |

`X.Y.Z` must be numeric; prerelease suffixes are optional.
