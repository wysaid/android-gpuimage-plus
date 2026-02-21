---
name: pr-review
description: Address review comments and CI failures for the current branch's PR
---

## Constraints

- Prepend `GH_PAGER=` to every `gh` command (bash/zsh), or set `$env:GH_PAGER=""` in PowerShell — never modify global config
- Key commands: `gh pr list --head <BRANCH>` · `gh pr view <PR> --comments` · `gh pr checks <PR>` · `gh run view <RUN_ID> --log-failed`
- Workflow fixes **cannot be verified locally** — the fix is only confirmed once the remote CI re-runs and passes

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
3. **Address review comments** — implement valid feedback; document why invalid ones are rejected
4. **Commit & push** — single commit covering all non-workflow fixes (workflow fixes are pushed incrementally during step 2)
5. **Final verification** — once all fixes are applied, confirm every check is green: `GH_PAGER= gh pr checks <PR>`

## Output

- Per CI failure: root cause and resolution (or escalation reason)
- Per review comment: resolution or rejection reason
- After all fixes: summary of check statuses confirming all green
