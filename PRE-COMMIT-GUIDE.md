# Pre-commit Hooks Guide for LGC2

This guide explains how to set up and use pre-commit hooks in the LGC2 project.

## Table of Contents

- [What are Pre-commit Hooks?](#what-are-pre-commit-hooks)
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

## Quick Start

For most users, simply run the setup script:

**Git Bash / Linux:**
```bash
./setup-pre-commit.sh
```

## Installation

### Windows

#### Option 1: Using the Setup Script (Recommended)

1. Open Git Bash or PowerShell in the LGC2 directory
2. Run the setup script:
   ```bash
   # Git Bash
   ./setup-pre-commit.sh


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
   ```

### Linux

1. Install Python 3 and pip (usually pre-installed):
   ```bash
   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install python3 python3-pip

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
```

### Skipping Hooks (Use Sparingly)

In rare cases where you need to skip hooks:
```bash
git commit --no-verify
```

**Warning:** Only use `--no-verify` when absolutely necessary, as it bypasses all quality checks.

## Signing Your Commits (REQUIRED)

### Developer Certificate of Origin (DCO)

Every commit in the LGC2 project **MUST** include a `Signed-off-by` line at the end of the commit message. This certifies that you have the right to submit the code under the project's license.

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

If you commit without the `-s` flag, the pre-commit hook will **reject** your commit with an error message:

```
❌ ERROR: Commit message must include a Signed-off-by line!

Add it automatically with: git commit -s

Or add this line manually:
Signed-off-by: Your Name <your.email@example.com>
```

To fix this:
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

### Commit Message Verification

- **check-signoff**: **REQUIRED** - Verifies that every commit message includes a `Signed-off-by` line
  - This is a Developer Certificate of Origin (DCO) requirement
  - Commits without proper sign-off will be rejected
  - Use `git commit -s` to add automatically

### Code Formatting

- **clang-format**: Automatically formats C++ code according to `.clang-format` configuration
  - Uses project-specific style (tabs, 170 char limit, etc.)
  - Modifies files in-place

### General Checks

- **check-added-large-files**: Prevents files larger than 1MB from being committed
- **check-case-conflict**: Prevents case-insensitive filename conflicts
- **check-merge-conflict**: Detects unresolved merge conflict markers
- **check-yaml**: Validates YAML syntax (e.g., `.gitlab-ci.yml`)
- **check-json**: Validates JSON syntax
- **end-of-file-fixer**: Ensures files end with a newline
- **trailing-whitespace**: Removes trailing whitespace
- **mixed-line-ending**: Ensures consistent line endings (LF)

### Project-Specific Hooks

- **check-todos**: Reports TODO/FIXME comments (informational only)
- **prevent-direct-master-commit**: Blocks direct commits to `master` or `appwidevs` branches

## CI/CD Integration

Pre-commit hooks and commit verification are automatically run in the GitLab CI pipeline:

### Pre-commit Job
- **Trigger**: On all merge requests and branch pushes (except `master` and `appwidevs`)
- **Stage**: `.pre` (runs before build stages)
- **Image**: `python:3.11-slim` with `clang-format` installed
- **Command**: `pre-commit run --all-files`

### Sign-off Check Job
- **Trigger**: On all merge requests and branch pushes (except `master` and `appwidevs`)
- **Stage**: `.pre` (runs before build stages)
- **Image**: `alpine/git:latest`
- **Purpose**: Verifies all commits in the MR/branch have `Signed-off-by` lines

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

**Problem:** clang-format is not installed.

**Solution:**
- **Windows:** Install LLVM from [llvm.org](https://releases.llvm.org/)
- **Linux:**
  ```bash
  # Ubuntu/Debian
  sudo apt-get install clang-format

  # CentOS/RHEL
  sudo yum install clang-tools-extra
  ```

### Hooks are Modifying My Files

**Expected behavior:** Some hooks (like `clang-format` and `trailing-whitespace`) automatically fix issues.

**What to do:**
1. Review the changes: `git diff`
2. If changes look good, stage them: `git add .`
3. Commit again: `git commit`

### Hooks are Too Slow

**Solution:**
- Run hooks only on changed files (default behavior when committing)
- Update pre-commit: `pre-commit autoupdate`
- Consider disabling expensive hooks locally (but they'll still run in CI)

### False Positives

If a hook incorrectly flags something:
1. Check if there's a configuration option to adjust it
2. Update `.pre-commit-config.yaml` to exclude specific files/patterns
3. Use `git commit --no-verify` as a last resort (but fix before pushing!)

## Best Practices

### For Developers

1. **Install hooks immediately** after cloning the repository
2. **Always sign your commits** with `git commit -s` (REQUIRED)
   - Consider creating an alias: `git config --global alias.c 'commit -s'`
3. **Run hooks before pushing** to catch issues early:
   ```bash
   pre-commit run --all-files
   ```
4. **Don't skip hooks** unless absolutely necessary
5. **Keep hooks updated**:
   ```bash
   pre-commit autoupdate
   ```
6. **Review auto-fixes** before committing

### For the Project

1. **Document hook changes** in commit messages
2. **Test new hooks locally** before adding to `.pre-commit-config.yaml`
3. **Keep hooks fast** - slow hooks reduce productivity
4. **Be consistent** - ensure local hooks match CI hooks
5. **Communicate changes** to the team when updating hook configuration

### Creating Feature Branches

Always create a feature branch before making changes:
```bash
git checkout -b sus-XXXX-feature-description
```

The pre-commit hooks will prevent you from accidentally committing directly to `master` or `appwidevs`.

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

To update all hooks to their latest versions:
```bash
pre-commit autoupdate
```

This updates the versions specified in `.pre-commit-config.yaml`.

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
```

### Disable a Hook

Comment it out in `.pre-commit-config.yaml`:
```yaml
# - id: some-hook  # Disabled because...
```

## Getting Help

- Pre-commit documentation: https://pre-commit.com/
- Report issues: Create a ticket in the [SUS Jira](https://its.cern.ch/jira/browse/SUS)
- Ask the team: Contact other LGC2 developers

---

**Remember:** Pre-commit hooks are here to help! They catch bugs, maintain consistency, and make code reviews faster. Embrace them as part of your development workflow.
