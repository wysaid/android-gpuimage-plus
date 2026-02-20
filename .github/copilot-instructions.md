# GitHub Copilot Instructions — android-gpuimage-plus

Android GPU filter library (C++ & Java). Prefer **small, safe, backward-compatible** changes.

## Stability Rules

- **Never break public API**: `org.wysaid.nativePort.*` Java signatures, JNI method names, and filter rule string syntax are stable contracts.
- Prefer **additive** changes over refactoring existing code.
- FFmpeg/video code must remain **fully optional** — guard with `disableVideoModule` (Java/Gradle) / `CGE_USE_VIDEO_MODULE` (C++).
- `cgeDemo/` is a demo app — never couple library APIs to demo-only code.

## Build

Use `tasks.sh` for all operations (`bash tasks.sh --help`). Key `local.properties` flags: `usingCMakeCompile`, `disableVideoModule`, `enable16kPageSizes`. See `docs/build.md` for details.

## Code Conventions

Language-specific rules live in `.github/instructions/code-conventions.instructions.md` and are auto-applied by file path.

## Skills

- **Submit PR:** Follow `.github/skills/pr-submit/SKILL.md` to create or update pull requests
- **Review PR:** Follow `.github/skills/pr-review/SKILL.md` to review pull requests

## Behavior Constraints

- **Validation:** After native code changes, ALWAYS run `bash tasks.sh --enable-cmake --build`.
- **Commit Policy:** Only commit to feature branches, never force-push to `master`. Write commit messages in English.
- **Completeness:** Implement fully or request clarification — no TODOs in committed code.
