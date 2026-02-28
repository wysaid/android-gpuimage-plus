---
name: pr-review
description: Address review comments and CI failures for the current branch's PR
---

## Constraints

- Prepend `GH_PAGER=` to every `gh` command (bash/zsh), or set `$env:GH_PAGER=""` in PowerShell — never modify global config
- Key commands: `gh pr list --head <BRANCH>` · `gh pr view <PR> --comments` · `gh pr checks <PR>` · `gh run view <RUN_ID> --log-failed`
- Workflow fixes **cannot be verified locally** — the fix is only confirmed once the remote CI re-runs and passes

### Resolving Review Threads

After fixing a P1/P2 comment, mark its thread as resolved on GitHub using the GraphQL mutation:

```bash
GH_PAGER= gh api graphql -f query='mutation { resolveReviewThread(input: { threadId: "PRRT_xxxx" }) { thread { isResolved } } }'
```

To get thread IDs with their status and a brief description:

```bash
GH_PAGER= gh api graphql -f query='
{
  repository(owner: "OWNER", name: "REPO") {
    pullRequest(number: PR_NUMBER) {
      reviewThreads(first: 50) {
        nodes {
          id
          isResolved
          isOutdated
          path
          line
          comments(first: 1) { nodes { body } }
        }
      }
    }
  }
}' | python3 -c "
import json,sys
data = json.load(sys.stdin)
for t in data['data']['repository']['pullRequest']['reviewThreads']['nodes']:
    body = t['comments']['nodes'][0]['body'][:100].replace('\n',' ') if t['comments']['nodes'] else ''
    print(f\"ID={t['id']}, resolved={t['isResolved']}, outdated={t['isOutdated']}, {t['path']}:{t['line']}\")
    print(f\"  >> {body}\")
"
```

Also resolve threads that are **outdated** (the underlying code they referenced has since changed), as they are no longer actionable.

### Review Restraint Policy

Before acting on any review comment or suggestion, classify it by importance:

| Level | Criteria | Action |
|-------|----------|--------|
| **P1 — Must Fix** | Bug, security issue, broken behavior, API contract violation, CI failure | Fix immediately |
| **P2 — Should Fix** | Correctness risk, meaningful maintainability improvement, clear code smell with real impact | Fix with brief justification |
| **P3 — Conditional** | Style preference, minor naming, "could be cleaner" | Fix if it aligns with best practices **and** the change is safe + trivial; defer if complex or has any potential functional impact |
| **P4 — Reject** | Contradicts project conventions, introduces unnecessary complexity, or is factually wrong | Reject with explanation |

**Rules:**
- For **P3**, apply this two-question test before touching the code:
  1. **Best practice?** — Does the change follow language/framework conventions (e.g. prefer imports over FQNs, use existing imports, standard patterns)?
  2. **Safe & trivial?** — Is the diff mechanical with zero risk of behavioral change and low effort?
  - Both **yes** → fix it silently, mark thread resolved.
  - Either **no** → do NOT modify code; record in the summary table and let the user decide.
- **Do not add comments to code** unless the comment explains non-obvious logic that is truly necessary. Never add comments just to acknowledge a review suggestion was applied.
- When the two-question test is ambiguous, prefer deferring rather than guessing.

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
   - P1/P2: implement the fix, then **immediately resolve the thread** using the GraphQL mutation above
   - Outdated threads: resolve them regardless of priority (no action needed, just mark resolved)
   - P3: apply the two-question test — fix + resolve if both answers are yes; otherwise record in summary and defer to user
   - P4: record rejection reason in summary
4. **Commit & push** — single commit covering all non-workflow fixes (workflow fixes are pushed incrementally during step 2)
5. **Final verification** — once all fixes are applied, confirm every check is green: `GH_PAGER= gh pr checks <PR>`

## Output

- Per CI failure: root cause and resolution (or escalation reason)
- After all fixes: summary of check statuses confirming all green
- **Review summary table** — produced at the end of every review session:

```markdown
| # | Source (comment / CI) | Issue description | Priority | Action taken | Reason if not fixed |
|---|-----------------------|-------------------|----------|--------------|---------------------|
| 1 | Reviewer @xxx         | use import instead of FQN | P3  | Fixed        | Best practice + trivial mechanical change |
| 2 | Reviewer @xxx         | rename internal var foo→bar | P3 | Not fixed  | Non-standard opinion, no best-practice backing — deferred to user |
| 3 | CI: lint              | null-check missing | P1      | Fixed        | —                   |
```

All P3/P4 items that were **not** fixed must appear in this table with a clear reason, so the user can make an informed decision.
