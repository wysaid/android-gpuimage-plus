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

## Publishing a Regular Release to Maven Central

1. Confirm `org.wysaid` is verified in [Central Portal](https://central.sonatype.com/publishing/namespaces) and that the public key for your local GPG signing key is available to Central. Follow the [namespace guide](https://central.sonatype.org/register/namespace/#by-dns) and the [GPG guide](https://central.sonatype.org/publish/requirements/gpg/).
2. Check source tags, [the existing Maven repository](https://maven.wysaid.org/org/wysaid/gpuimage-plus/), [Maven Central](https://repo.maven.apache.org/maven2/org/wysaid/gpuimage-plus/), and the version used by Google Play SDK ownership verification before choosing a version. Use a regular release version for Central. The Play verification resource belongs to the version reserved for Play on `maven.wysaid.org`.
3. Set `versionName` in [build.gradle](../build.gradle) and run the `Build and Release` workflow manually on that ref with version `X.Y.Z`. Download `maven-central-X.Y.Z-unsigned` and review the four versions (`X.Y.Z`, `X.Y.Z-min`, `X.Y.Z-16k`, and `X.Y.Z-16k-min`). Each must have an AAR, a POM, a sources JAR, and a Javadoc JAR. Check the CI logs for ABI and ELF validation. The POM should declare AAR packaging and carry the runtime AppCompat dependency. CameraX remains opt-in.
4. For a release served by both repositories, use the unsigned bundle from the successful **tag-triggered run that synced the self-hosted repository**. That run exports and syncs the same Maven files. Verify the public self-hosted files match before signing the bundle. The run artifacts expire after seven days.
5. Unpack the unsigned ZIP and sign it with the local GPG key. The script adds detached signatures and computes the MD5 and SHA-1 files for all four Maven versions.

   ```bash
   VERSION=X.Y.Z
   unzip "maven-central-$VERSION-unsigned.zip" -d central-staging
   python3 tools/central_bundle.py central-staging "$VERSION" \
       "maven-central-$VERSION.zip" --signing-key "$GPG_SIGNING_KEY_FINGERPRINT"
   ```

6. Upload the signed ZIP in [Central Portal](https://central.sonatype.com/publishing/deployments). Review all four coordinates after validation passes, then manually publish the deployment. Record the configuration PR, source commit, CI run, coordinates, and deployment ID. [Portal upload guide](https://central.sonatype.org/publish/publish-portal-upload/)
7. Once the files are publicly downloadable, use a fresh Gradle user home and a standalone Android consumer configured with `mavenCentral()` and `google()`. Resolve each version individually and build the app with at least `X.Y.Z` and `X.Y.Z-16k`. Update the README dependency example after that check succeeds.

## Tag Format

| Type | Format | Example |
|------|--------|---------|
| Official | `vX.Y.Z` | `v3.1.1` |
| Beta | `vX.Y.Z-beta#` | `v3.1.1-beta1` |
| Alpha | `vX.Y.Z-alpha#` | `v3.1.1-alpha1` |
| RC | `vX.Y.Z-rc#` | `v3.1.1-rc1` |

`X.Y.Z` must be numeric; prerelease suffixes are optional.
