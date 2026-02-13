---
name: pr-review
description: Address review comments from GitHub PR using gh CLI and resolve feedback
---

## When to use

- Current branch has a PR with review comments requiring responses
- Need to systematically evaluate and resolve reviewer feedback

## Constraints

- **ALWAYS** set `PAGER=cat` before calling `gh` to avoid pagination issues
- Use `gh pr view <PR_NUMBER> --comments` to fetch review comments
- Use `gh pr list --head <CURRENT_BRANCH>` to find the PR number for the current branch

## Procedure

1. **Find Issues**: Use `gh` to locate the PR for current branch and retrieve all review comments
2. **Evaluate**: Analyze each comment in project context:
   - If valid: implement the necessary code improvements
   - If invalid: document reasons and address any underlying real issues
3. **Summarize**: Provide a summary covering:
   - Valid points and how they were resolved
   - Invalid points and reasons for rejection  
   - Any additional issues discovered and fixed
4. **Commit**: Generate commit message, then `git commit` and `git push`

## Output

- Clear mapping between review comments and their resolutions