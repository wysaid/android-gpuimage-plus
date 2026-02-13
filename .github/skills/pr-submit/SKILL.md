---
name: pr-submit
description: Create or update a GitHub Pull Request after completing code changes
---

## When to use

- Code changes are complete and ready for review
- Need to create a new PR or update an existing one with new commits

## Constraints

- **ALWAYS** set `PAGER=cat` before calling `gh` to avoid pagination issues

## Procedure

1. **Verify**: Review conversation history and current changes—ensure completeness, correctness, no performance issues
2. **Clean**: Remove temporary files, debug artifacts, and unintended changes
3. **Commit & Push**:
   - If on `master`, create a new feature branch (avoid name conflicts)
   - Commit all changes and push to remote
4. **PR**: Use `gh` to create or update the Pull Request:
   - Check if current branch has an existing PR
   - **If exists**: Fetch title/description, incorporate new commits, update the PR
   - **If not**: Create PR against remote `master`, draft title/description from changes

## Output

- Focus on: what changed, why, impact, verification