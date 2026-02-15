# Contributing Guide

## Getting Started

1. Fork the repository
2. Create a feature branch from `master` (`feat/xxx`, `fix/xxx`, `refactor/xxx`)
3. Make your changes
4. Verify the build: `bash tasks.sh --enable-cmake --build`
5. Submit a pull request to `master`

## Code Standards

- Use **English** for all code comments, commit messages, and PR descriptions
- Follow conventional-commit format: `feat:`, `fix:`, `refactor:`, `chore:`
- One logical change per commit
- Detailed code style rules are automatically applied by file path — see `.github/instructions/`

## Compatibility Rules

These are **hard constraints** for the public API:

- **Do not change** method signatures in `org.wysaid.nativePort.*`
- **Do not change** existing JNI function names
- **Do not change** existing filter rule string syntax
- New methods, filters, and syntax additions are welcome

## Key Constraints

- **FFmpeg/Video Module**: All video-related code must be optional — verify builds work with `--disable-video-module`
- **Dual Build**: When adding native files, CMake auto-discovers via `GLOB_RECURSE`, but `Android.mk` requires explicit listing
- **Thread Safety**: GL operations must run on the GL thread (`queueEvent(...)` in Java)

See `copilot-instructions.md` for detailed project conventions.

## CI

PRs trigger CI on three platforms (macOS, Ubuntu, Windows) with a reduced matrix. Merges to `master` run the full 16-combination matrix. Ensure all checks pass before requesting review.
