---
name: pr-review
description: Address review comments and CI failures for the current branch's PR
---

## Constraints

- Prepend `GH_PAGER=` to every `gh` command (bash/zsh), or set `$env:GH_PAGER=""` in PowerShell — never modify global config
- Key commands: `gh pr list --head <BRANCH>` · `gh pr view <PR> --comments` · `gh pr checks <PR>` · `gh run view <RUN_ID> --log-failed`
- Workflow fixes **cannot be verified locally** — the fix is only confirmed once the remote CI re-runs and passes

### Review Restraint Policy

Before acting on any review comment or suggestion, classify it by importance:

| Level | Criteria | Action |
|-------|----------|--------|
| **P1 — Must Fix** | Bug, security issue, broken behavior, API contract violation, CI failure | Fix immediately |
| **P2 — Should Fix** | Correctness risk, meaningful maintainability improvement, clear code smell with real impact | Fix with brief justification |
| **P3 — Optional** | Style preference, minor naming nitpick, debatable design choice, "could be cleaner" | **Do NOT auto-fix** — surface to user for decision |
| **P4 — Reject** | Contradicts project conventions, introduces unnecessary complexity, or is factually wrong | Reject with explanation |

**Rules:**
- **Do not implement P3 suggestions automatically.** Note them in the final summary and let the user decide.
- **Do not add comments to code** unless the comment explains non-obvious logic that is truly necessary. Never add comments just to acknowledge a review suggestion was applied.
- When in doubt about importance, prefer the lower severity (P3/P4) and defer to the user rather than making the change.

## Procedure

1. **Locate PR** — get PR number for current branch
2. **Fix CI failures** — for each failing check:
   - Fetch logs: `gh run view <RUN_ID> --log-failed`
   - **Workflow issue** (wrong config, missing step, bad path):
     1. Fix `.github/workflows/` directly
     2. Commit & push the workflow change
     3. Wait for the re-triggered run to complete: poll with `GH_PAGER= gh pr checks <PR>` (or `GH_PAGER= gh run watch <RUN_ID>`) until the affected check finishes
     4. If it **passes** → continue to the next failing check
     5. If it **still fails** → fetch new logs (`gh run view <NEW_RUN_ID> --log-failed`) and repeat from step i
   - **Code issue, non-breaking**: fix source code directly
   - **Code issue, breaking change required**: stop and report to developer for a decision
3. **Address review comments** — apply the Review Restraint Policy to each comment:
   - P1/P2: implement the fix
   - P3: record in summary, do **not** modify code, ask user
   - P4: record rejection reason in summary
4. **Commit & push** — single commit covering all non-workflow fixes (workflow fixes are pushed incrementally during step 2)
5. **Final verification** — once all fixes are applied, confirm every check is green: `GH_PAGER= gh pr checks <PR>`

## Output

- Per CI failure: root cause and resolution (or escalation reason)
- After all fixes: summary of check statuses confirming all green
- **Review summary table** — produced at the end of every review session:

```
| # | Source (comment / CI) | Issue description | Priority | Action taken | Reason if not fixed |
|---|-----------------------|-------------------|----------|--------------|---------------------|
| 1 | Reviewer @xxx         | e.g. rename foo() | P3       | Not fixed    | Style preference, no functional impact — deferred to user |
| 2 | CI: lint              | e.g. null-check   | P1       | Fixed        | —                   |
```

All P3/P4 items that were **not** fixed must appear in this table with a clear reason, so the user can make an informed decision.
