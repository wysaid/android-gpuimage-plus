---
name: pr-submit
description: Create or update a GitHub Pull Request after completing code changes
---

## When to Use

- Code changes are complete and ready for review
- Need to create a new PR or update an existing one

## Constraints

- PR title, description, and comments **must** be in English
- Follow conventional-commit format for titles: `feat:`, `fix:`, `refactor:`, `chore:`
- Target branch: `master`
- **Never** force-push to `master`
- Follow `.github/CONTRIBUTING.md` for code standards and compatibility rules

## Procedure

1. **Verify Build**: Run `bash tasks.sh --enable-cmake --build`
   - If touching FFmpeg/video code: verify with `--disable-video-module`
   - If new native files added: confirm `Android.mk` updated
2. **Check Compatibility**: Review `.github/CONTRIBUTING.md` compatibility rules
   - Confirm no public API signature changes in `org.wysaid.nativePort.*`
3. **Branch & Commit**:
   - Create feature branch if on `master` (`feat/`, `fix/`, `refactor/`)
   - Follow conventional-commit format (see CONTRIBUTING.md)
4. **Create PR**: Push branch and create PR against `master`
   - Use conventional-commit format for title
   - Fill in description template below
5. **Verify CI**: Ensure all platform workflows pass

## PR Description Template

```markdown
## Summary

Brief description of what this PR does and why.

## Changes

- Bullet list of key changes

## Compatibility

- [ ] No public API changes (`org.wysaid.nativePort.*`)
- [ ] No filter rule string syntax changes
- [ ] FFmpeg/video code guarded with `disableVideoModule` / `CGE_USE_VIDEO_MODULE` (or N/A)
- [ ] Both CMake and ndk-build updated (or N/A — no new native source files)

## Testing

How was this tested (e.g., "built with CMake + no-ffmpeg on macOS", "ran cgeDemo on Pixel 7").
```

## Output

- PR with English title and description
- All CI checks passing
