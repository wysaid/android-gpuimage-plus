---
name: pr-review
description: Review a GitHub Pull Request for API stability, build compliance, native code safety, and code quality
---

## When to Use

- Need to review a pull request before merge
- Validate PR changes against project rules
- Provide structured review feedback

## Constraints

- All review comments **must** be in English
- Work through checks **in order** — stop and flag any blocker immediately
- Reference `.github/CONTRIBUTING.md` for detailed code standards

## Procedure

Follow `.github/CONTRIBUTING.md` for detailed rules. Check in order:

1. **API Stability** (Blocker):
   - No changes to `org.wysaid.nativePort.*` method signatures
   - No changes to JNI function names
   - Filter rule string syntax backward-compatible
2. **Build Toggle Compliance** (Blocker):
   - FFmpeg/video code properly guarded (C++: `#ifdef CGE_USE_VIDEO_MODULE`, Java: `BuildConfig.CGE_USE_VIDEO_MODULE`)
   - Compiles with `disableVideoModule=true`
3. **Native Code Safety** (High):
   - No C++ exceptions, JNI null checks, GL resource cleanup, shader error handling
4. **Dual Build System** (Medium):
   - New native files → `Android.mk` updated
5. **Code Quality**: Follow CONTRIBUTING.md standards
6. **CI Status**: All platform workflows pass

## Output

Structure the review as:

```markdown
## PR Review: <PR title>

### Summary
One-sentence assessment: Approve / Request Changes / Needs Discussion

### Findings

#### Blockers
- (API stability or build toggle violations)

#### Issues
- (correctness, safety, or quality problems)

#### Suggestions
- (optional improvements, not blocking)

### Verdict
APPROVE / REQUEST_CHANGES with rationale
```

## Severity Guide

| Severity | Criteria | Action |
|----------|----------|--------|
| **Blocker** | API break, build toggle violation, crash bug | Request changes |
| **Issue** | Memory/GL resource leak, missing null check, wrong thread | Request changes if risky, else comment |
| **Suggestion** | Style, naming, minor optimization | Comment only |
