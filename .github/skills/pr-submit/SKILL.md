---
name: pr-submit
description: Create or update a GitHub Pull Request — use ONLY when explicitly asked to create/submit/open a PR
---

## When to use

**Only** when the user explicitly asks to create, submit, or open a Pull Request.
Do NOT invoke this skill for plain commit or push requests.

## Constraints

- Prepend `GH_PAGER=` to every `gh` command (bash/zsh), or set `$env:GH_PAGER=""` in PowerShell — never modify global config

## Procedure

1. **Verify & clean** — ensure changes are complete, correct, and free of debug artifacts
2. **Commit & push**:
   - If on `master`, create a new feature branch first (avoid name conflicts)
   - Commit all changes and push to remote
3. **Create or update PR**:
   - Check if current branch already has a PR (`gh pr list --head <BRANCH>`)
   - **Exists**: update title/description to reflect new commits
   - **None**: create PR against `master` with title/description derived from changes

## Output

- What changed, why, impact, and how to verify