# Pre-commit Hooks Guide for LGC2

This guide explains how to set up and use pre-commit hooks in the LGC2 project.

## Table of Contents

- [What are Pre-commit Hooks?](#what-are-pre-commit-hooks)
- [Hook Architecture](#hook-architecture)
- [Quick Start](#quick-start)
- [Installation](#installation)
  - [Windows](#windows)
  - [Linux](#linux)
- [Usage](#usage)
- [Signing Your Commits (REQUIRED)](#signing-your-commits-required)
- [Configured Hooks](#configured-hooks)
- [CI/CD Integration](#cicd-integration)
- [Troubleshooting](#troubleshooting)
- [Best Practices](#best-practices)

## What are Pre-commit Hooks?

Pre-commit hooks are automated checks that run before each commit to ensure code quality and consistency. They help catch issues early, maintain code standards, and reduce review time.

## Hook Architecture

LGC2 does **not** define custom checks as `repo: local` anymore (except optional project-only experiments). Hooks come from three places, selected in [`.pre-commit-config.yaml`](./.pre-commit-config.yaml):

| Category | Source | Role |
|----------|--------|------|
| **1. Upstream** | Public hook repos (e.g. `pre-commit/pre-commit-hooks`, `astral-sh/ruff-pre-commit`) | Pin well-maintained open-source tools directly |
| **2. Shared APC** | [`apc/common/pre-commit-utils`](https://gitlab.cern.ch/apc/common/pre-commit-utils) | Reusable APC policy hooks (REUSE, UTF-8, DCO, protected branches, …) |
| **3. LGC2-specific** | Project pin (e.g. `mirrors-clang-format` + `.clang-format`) | Choices that belong to this codebase only |

A catalogue of recommended upstream hooks is maintained in pre-commit-utils:
https://gitlab.cern.ch/apc/common/pre-commit-utils/-/blob/master/docs/catalogue.md

Shared APC hooks are **published** by that repo (via `.pre-commit-hooks.yaml`); LGC2 only **selects** them by `id` and pins `rev`. pre-commit does not support importing YAML config fragments.

## Quick Start

For most users, simply run the setup script:

**Git Bash / Linux:**
```bash
./setup-pre-commit.sh
```

This installs both the `pre-commit` and `commit-msg` git hooks (the latter is required for `check-signoff`).

## Installation

### Windows

#### Option 1: Using the Setup Script (Recommended)

1. Open Git Bash or PowerShell in the LGC2 directory
2. Run the setup script:
   ```bash
   # Git Bash
   ./setup-pre-commit.sh
   ```

#### Option 2: Manual Installation

1. Install Python 3.6+ from [python.org](https://www.python.org/downloads/)
   - Make sure to check "Add Python to PATH" during installation
2. Install pre-commit:
   ```bash
   pip install pre-commit
   ```
3. Navigate to the LGC2 directory and install hooks:
   ```bash
   cd C:\path\to\LGC2
   pre-commit install
   pre-commit install --hook-type commit-msg
   ```

### Linux

1. Install Python 3 and pip (usually pre-installed):
   ```bash
   # Ubuntu/Debian
   sudo apt-get update && sudo apt-get install python3 python3-pip

   # CentOS/RHEL
   sudo yum install python3 python3-pip
   ```

2. Install pre-commit:
   ```bash
   pip3 install --user pre-commit
   ```

3. Install hooks:
   ```bash
   cd /path/to/LGC2
   pre-commit install
   pre-commit install --hook-type commit-msg
   ```

## Usage

### Automatic Execution

Once installed, pre-commit hooks run automatically before each `git commit`. If any hook fails:
- The commit is blocked
- Files may be automatically fixed (e.g., formatting)
- You need to review changes and commit again

### Manual Execution

Run all hooks on all files:
```bash
pre-commit run --all-files
```

Run hooks on specific files:
```bash
pre-commit run --files source/LGC_Core/sources/main.cpp
```

Run a specific hook:
```bash
pre-commit run clang-format --all-files
pre-commit run prevent-direct-branch-commit --all-files
```

### Skipping Hooks (Use Sparingly)

In rare cases where you need to skip hooks:
```bash
git commit --no-verify
```

**Warning:** Only use `--no-verify` when absolutely necessary, as it bypasses all quality checks.

## Signing Your Commits

### Developer Certificate of Origin (DCO)

All commits that will be merged to main branches (master/appwidevs) **MUST** include a `Signed-off-by` line at the end of the commit message. This certifies that you have the right to submit the code under the project's license.

### When is Sign-off Required?

- **Development branches**: Sign-off is **recommended** but not enforced. You can make intermediate commits without sign-off.
- **Merge requests**: Sign-off is **required** for all commits. Before creating a merge request, ensure all commits are signed.
- **Best practice**: Use `git commit -s` for all commits to avoid having to sign them later.

### How to Sign Your Commits

**Always use the `-s` flag when committing:**

```bash
git commit -s -m "Your commit message"
```

This automatically adds the following line to your commit message:
```
Signed-off-by: Your Name <your.email@example.com>
```

The name and email are taken from your git configuration:
```bash
git config user.name
git config user.email
```

### Setting Up Your Git Identity

If you haven't configured your git identity yet:

```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

For CERN users, use your CERN email address.

### What If I Forget to Sign?

If you commit without the `-s` flag, the shared `check-signoff` hook (commit-msg stage) will **warn** you:

```
Warning: Commit message does not include a Signed-off-by line.

This is not required for development commits, but ALL commits
must be signed-off before creating a merge request.
```

You can:
- Skip the warning for development commits: `git commit --no-verify`
- Sign the commit immediately:
1. Amend your last commit with the sign-off:
   ```bash
   git commit --amend -s --no-edit
   ```

2. Or, if you need to edit the message:
   ```bash
   git commit --amend -s
   ```

### Signing Multiple Commits

If you have multiple commits without sign-off, you can rebase and sign them:

```bash
# Interactive rebase for last N commits
git rebase -i HEAD~N

# For each commit, mark it with 'edit' or 'reword'
# Then for each:
git commit --amend -s --no-edit
git rebase --continue
```

### Why Is This Required?

The Signed-off-by line is a lightweight way to certify that you wrote the code or have the right to submit it. It's based on the [Developer Certificate of Origin](https://developercertificate.org/), which is widely used in open source projects.

## Configured Hooks

Hooks below match [`.pre-commit-config.yaml`](./.pre-commit-config.yaml). Names and ownership follow the three categories above.

### 1. Upstream hooks

From `pre-commit/pre-commit-hooks`:

- **check-added-large-files**: Prevents files larger than 1MB from being committed
- **check-case-conflict**: Prevents case-insensitive filename conflicts
- **check-merge-conflict**: Detects unresolved merge conflict markers
- **check-yaml**: Validates YAML syntax (e.g., `.gitlab-ci.yml`; uses `--unsafe` for GitLab CI tags)
- **check-json**: Validates JSON syntax
- **end-of-file-fixer**: Ensures files end with a newline (excludes `.txt` test outputs)
- **trailing-whitespace**: Removes trailing whitespace (excludes `.txt` test outputs)
- **mixed-line-ending**: Ensures consistent line endings (LF)

From `astral-sh/ruff-pre-commit`:

- **ruff-check**: Lints Python code with [Ruff](https://docs.astral.sh/ruff/)
- **ruff-format**: Formats Python code with Ruff (Black-compatible style); modifies files in-place

### 2. Shared APC hooks (`apc/common/pre-commit-utils`)

These are **not** local hooks. They are installed from the shared repo when you run `pre-commit install` / CI `pre-commit run`.

- **check-utf8-encoding**: Ensures matched text files are valid UTF-8
- **reuse-committed-files**: REUSE `lint-file` on each committed file
  - `reuse[charset-normalizer]` is an **additional dependency of this hook** (managed by pre-commit). You do not need to `pip install reuse` yourself or in CI.
- **check-todos**: Reports `TODO`/`FIXME` in staged files (informational; does not fail)
- **prevent-direct-branch-commit**: Blocks commits on protected branches
  - LGC2 configures: `args: [--branch, master, --branch, appwidevs]`
  - (Renamed from the old local id `prevent-direct-master-commit`)
- **check-signoff**: Requires a `Signed-off-by:` line (`commit-msg` stage)
  - Needs `pre-commit install --hook-type commit-msg`
  - Local: can skip with `--no-verify`
  - CI: separate `check-signoff` job still enforces sign-off on merge requests

### 3. LGC2-specific hooks

- **clang-format**: Formats C/C++ using the project `.clang-format` (`-style=file`)
  - Version pinned to v22.1.5 via `pre-commit/mirrors-clang-format` — no separate system install needed for the hook
  - **The version matters**: same `.clang-format` config produces different output across versions; v22.1.5 is the project standard
  - IDE formatters (e.g. Visual Studio "Format Document") must also use v22.1.5 to avoid spurious diffs — see troubleshooting below
  - Modifies files in-place

Optional project-only checks can be added later under `repo: local` in `.pre-commit-config.yaml`.

## CI/CD Integration

Pre-commit hooks and commit verification are automatically run in the GitLab CI pipeline:

### Pre-commit Job
- **Trigger**: On all merge requests and branch pushes (except `master` and `appwidevs`)
- **Stage**: `.pre` (runs before build stages)
- **Image**: `python:3.11-slim`
- **Install**: `pip install pre-commit` only — hook tools (Ruff, clang-format mirror, `reuse`, …) are installed into pre-commit’s own environments from each hook’s `repo` / `additional_dependencies`
- **Scope**: Only checks files modified in the branch/MR (not entire codebase)
- **Purpose**: Ensures new/modified code meets quality standards
- **Note**: The job must be able to clone `apc/common/pre-commit-utils` (same GitLab group access as other APC common projects)

### Sign-off Check Job
- **Trigger**: Only on merge requests (not on regular branch pushes)
- **Stage**: `.pre` (runs before build stages)
- **Image**: `alpine/git:latest`
- **Purpose**: Verifies all commits in the merge request have `Signed-off-by` lines
- **Note**: This only runs when you create a merge request, allowing unsigned commits on development branches. It is independent of the shared `check-signoff` pre-commit hook.

If either stage fails in CI, your merge request cannot be merged until the issues are resolved.

### Viewing CI Results

1. Go to your merge request in GitLab
2. Check the pipeline status
3. If `pre-commit` job fails: Click on it to see which hooks failed
4. If `check-signoff` job fails: One or more commits are missing `Signed-off-by`
5. Fix the issues locally and push again

**To fix unsigned commits:**
```bash
# For the last commit
git commit --amend -s --no-edit
git push --force-with-lease

# For multiple commits
git rebase -i origin/appwidevs -x 'git commit --amend -s --no-edit'
git push --force-with-lease
```

## Troubleshooting

### "Commit message must include a Signed-off-by line"

**Problem:** You forgot to use `git commit -s`.

**Solution:**
1. Amend your commit with the sign-off:
   ```bash
   git commit --amend -s --no-edit
   ```

2. Or configure a git alias to always sign:
   ```bash
   git config --global alias.c 'commit -s'
   # Now use: git c -m "message" instead of git commit -m "message"
   ```

3. To sign all commits in a branch:
   ```bash
   git rebase -i HEAD~N  # N = number of commits
   # Mark each with 'edit', then for each:
   git commit --amend -s --no-edit
   git rebase --continue
   ```

### "pre-commit: command not found"

**Problem:** The `pre-commit` command is not in your PATH.

**Solution:**
- **Windows:** Add Python Scripts directory to PATH:
  ```
  %APPDATA%\Python\Python3x\Scripts
  ```
- **Linux:** Add to `~/.bashrc`:
  ```bash
  export PATH="$HOME/.local/bin:$PATH"
  ```
  Then run: `source ~/.bashrc`

### "clang-format: command not found"

clang-format is managed by pre-commit via `mirrors-clang-format` and does not need a separate system install. If you see this error, run `pre-commit install` to (re)initialize the environment — pre-commit will download the pinned binary automatically.

### IDE formatter produces different output than pre-commit

**Problem:** Visual Studio (or another IDE) uses a different clang-format version than the pinned v22.1.5, causing spurious diffs.

**Option A — pip install (stable path, recommended):**

```
pip install clang-format==22.1.5
```

The binary lands at:
- **Windows:** `%USERPROFILE%\AppData\Local\Programs\Python\Python3xx\Scripts\clang-format.exe`
- **Linux:** `~/.local/bin/clang-format`

Find the exact path with:
```powershell
# Windows
(Get-Command clang-format).Source
```
```bash
# Linux
which clang-format
```

When the pinned version in `.pre-commit-config.yaml` changes, run `pip install clang-format==<new_version>` to stay in sync.

**Option B — pre-commit virtualenv (no extra install):**

After running `pre-commit install`, find the v22.1.5 binary inside the pre-commit cache:
```powershell
# Windows
Get-ChildItem -Recurse "$env:USERPROFILE\.cache\pre-commit" -Filter "clang-format.exe" |
  Where-Object { & $_.FullName --version 2>&1 | Select-String "22\." } |
  Select-Object FullName
```
```bash
# Linux
find ~/.cache/pre-commit -name "clang-format" -executable -type f |
  while read f; do v=$("$f" --version 2>&1); echo "$v $f"; done | grep "^clang-format version 22"
```

Note: this path contains a hash that changes when pre-commit rebuilds its environment (e.g. after `pre-commit clean`). Re-run the command above and update VS if that happens.

**Configure Visual Studio** (both options): **Tools → Options → Text Editor → C/C++ → Formatting → General → "Use custom clang-format.exe file"** → select the binary path.

The binary reads `.clang-format` from the project root via the normal directory walk — no extra configuration needed.

### Hooks are Modifying My Files

**Expected behavior:** Some hooks (like `clang-format`, `ruff-format`, and `trailing-whitespace`) automatically fix issues.

**What to do:**
1. Review the changes: `git diff`
2. If changes look good, stage them: `git add .`
3. Commit again: `git commit -s`

### Hooks are Too Slow

**Solution:**
- Run hooks only on changed files (default behavior when committing)
- Update pre-commit: `pre-commit autoupdate`
- Consider disabling expensive hooks locally (but they'll still run in CI)

### CI Job Fails: "files were modified by this hook"

**Problem:** Pre-commit hooks are modifying files in CI, causing the job to fail.

**Cause:** Files in your branch don't meet the formatting/quality standards, so hooks auto-fix them.

**Solution:**
1. Run pre-commit locally before pushing:
   ```bash
   pre-commit run --files path/to/modified/files.cpp
   ```

2. Review and commit the auto-fixed changes:
   ```bash
   git add .
   git commit -s -m "Apply pre-commit auto-fixes"
   ```

3. Push again - the CI should now pass

**Note:** The CI only checks files you modified, not the entire codebase, so you only need to fix files you're actually changing.

### File is Not Valid UTF-8

**Problem:** Pre-commit hook reports "ERROR: filename is not valid UTF-8"

**Cause:** The file contains non-UTF-8 characters or uses a different encoding (e.g., ISO-8859-1, Windows-1252).

**Solution:**

1. **Using a text editor:**
   - Open the file in VS Code, Notepad++, or your preferred editor
   - Save with UTF-8 encoding (usually in File > Save with Encoding)

2. **Using command line (Linux/Git Bash):**
   ```bash
   # Convert from ISO-8859-1 to UTF-8
   iconv -f ISO-8859-1 -t UTF-8 input.cpp > output.cpp
   mv output.cpp input.cpp
   ```

3. **Check file encoding:**
   ```bash
   file -b --mime-encoding filename.cpp
   ```

### Cannot clone `apc/common/pre-commit-utils`

**Problem:** Local or CI `pre-commit` fails while fetching the shared hooks repo.

**Cause:** Missing GitLab access to `apc/common/pre-commit-utils`, or an outdated pinned `rev`.

**Solution:**
- Confirm you (or the CI job token) can browse/clone that project
- Update the `rev:` pin in `.pre-commit-config.yaml` to a known-good tag or commit from that repo

### False Positives

If a hook incorrectly flags something:
1. Check if there's a configuration option to adjust it
2. Update `.pre-commit-config.yaml` to exclude specific files/patterns
3. Use `git commit --no-verify` as a last resort (but fix before pushing!)

## Best Practices

### For Developers

1. **Install hooks immediately** after cloning the repository (`pre-commit` + `commit-msg`)
2. **Always sign your commits** with `git commit -s` (REQUIRED)
   - Consider creating an alias: `git config --global alias.c 'commit -s'`
3. **Run hooks before pushing** to catch issues early:
   ```bash
   pre-commit run --all-files
   ```
4. **Don't skip hooks** unless absolutely necessary
5. **Keep hooks updated** carefully:
   ```bash
   pre-commit autoupdate
   ```
   Prefer bumping shared APC hooks by changing the pinned `rev` of `pre-commit-utils` after reviewing that project's changelog/tags.
6. **Review auto-fixes** before committing

### For the Project

1. **Document hook changes** in commit messages
2. **Prefer shared APC hooks** for policy that other projects need; keep LGC-only choices in category 3
3. **Test new hooks locally** before adding to `.pre-commit-config.yaml`
4. **Keep hooks fast** - slow hooks reduce productivity
5. **Be consistent** - ensure local hooks match CI hooks
6. **Communicate changes** to the team when updating hook configuration or the `pre-commit-utils` pin

### Creating Feature Branches

Always create a feature branch before making changes:
```bash
git checkout -b sus-XXXX-feature-description
```

The `prevent-direct-branch-commit` hook blocks accidental commits to `master` or `appwidevs`.

**Remember to always sign your commits:**
```bash
git commit -s -m "Your feature implementation"
```

### Handling Large Refactorings

When doing large-scale refactoring that affects many files:

1. Run pre-commit on all files first:
   ```bash
   pre-commit run --all-files
   ```
2. Commit formatting changes separately from functional changes
3. Consider temporarily disabling certain hooks if they're causing issues (discuss with team first)

## Updating Hooks

To update pinned revisions in `.pre-commit-config.yaml`:
```bash
pre-commit autoupdate
```

For shared APC hooks, prefer an explicit bump of the `apc/common/pre-commit-utils` `rev` (tag or SHA) after reviewing upstream changes.

## Customization

To modify hook behavior, edit `.pre-commit-config.yaml`. Common customizations:

### Exclude Files from a Hook

```yaml
- id: clang-format
  exclude: 'lib/external/'  # Exclude external libraries
```

### Change Hook Arguments

```yaml
- id: check-added-large-files
  args: ['--maxkb=2000']  # Increase file size limit

- id: prevent-direct-branch-commit
  args: [--branch, master, --branch, appwidevs]
```

### Disable a Hook

Comment it out in `.pre-commit-config.yaml`:
```yaml
# - id: some-hook  # Disabled because...
```

## Getting Help

- Pre-commit documentation: https://pre-commit.com/
- Shared APC hooks: https://gitlab.cern.ch/apc/common/pre-commit-utils
- Report issues: Create a ticket in the [SUS Jira](https://its.cern.ch/jira/browse/SUS)
- Ask the team: Contact other LGC2 developers

---

**Remember:** Pre-commit hooks are here to help! They catch bugs, maintain consistency, and make code reviews faster. Embrace them as part of your development workflow.
