# English Release Guide for LGC2

This guide explains how to produce the English build of LGC2 (`vX.Y.Z-eng`) from a regular
release (`vX.Y.Z`).

## Table of Contents

- [Principle](#principle)
- [One-time Setup](#one-time-setup)
- [Porting to a New Release](#porting-to-a-new-release)
- [Checks Before Tagging](#checks-before-tagging)
- [Publishing](#publishing)
- [History](#history)

## Principle

The English version is **one commit** on top of the official release tag. Each English
release has its own branch, created from the previous English tag and rebased onto the new
release tag:

```
v2.11.0 ── E                    eng/v2.11.0   (tag v2.11.0-eng)
   │
   └─ ... ── v2.11.1 ── E'      eng/v2.11.1   (tag v2.11.1-eng)
```

The commit `[SUS-2594] English version X.Y.Z-eng` contains:

- the translated writers, keywords and comments,
- the `rules:` of `windows64_release` in `.gitlab-ci.yml` (installer built on `-eng` tags),
- this guide,
- the version `X.Y.Z-eng` in `source/CMakeLists.txt` and `source/LGC_Core/LGCCore.cmake`.

Rules:

- **Never rewrite a branch or tag that was released.** Always create a new branch; the old tag
  keeps pointing at the old commit.
- Keep it one commit: every change is folded in with `git commit --amend`, never added on top.
- Bug fixes and features that are not about language belong in `appwidevs`, not here.
- Naming: branch `eng/vX.Y.Z`, tag `vX.Y.Z-eng`.

## One-time Setup

Make Git remember how you resolved conflicts, so the same conflict is solved automatically in
the next port:

```bash
git config rerere.enabled true
```

## Porting to a New Release

`<OLD>` is the previous release (e.g. `v2.11.1`), `<NEW>` the new one (e.g. `v2.12.0`).
Start from a clean working tree.

```bash
git fetch --tags origin

# 1. New branch from the previous English release
git switch -c eng/<NEW> <OLD>-eng

# 2. Replay the English commit (the one after <OLD>) on top of <NEW>
git rebase --onto <NEW> <OLD> eng/<NEW>
```

`--onto <NEW> <OLD>` means "take the commits after `<OLD>` on this branch and replay them
on `<NEW>`". Here that is only the English commit.

Resolving conflicts:

- **Writers** (`source/LGC_Core/sources/Writers/`, `source/LGC_Core/sources/LGCObjectWriters/`)
  cause most of the conflicts: keep the new code from `<NEW>` and translate its strings again.
- **Version files** (`source/CMakeLists.txt`, `source/LGC_Core/LGCCore.cmake`): take the
  `<NEW>` side (`git checkout --ours -- <file>`; during a rebase "ours" is the new base) and
  redo the bump in step 3.
- Check every resolved file for encoding: files are UTF-8, with LF line endings.
- Continue with `git add <file>` and `git rebase --continue`. Stop with `git rebase --abort` if
  things go wrong; the branch goes back to how it was before.

```bash
# 3. Version bump, folded into the English commit
#    source/CMakeLists.txt           set (EXE_VERSION_PATCH "<patch>-eng")
#    source/LGC_Core/LGCCore.cmake   set (LGC_VERSION_PATCH "<patch>-eng")
git add source/CMakeLists.txt source/LGC_Core/LGCCore.cmake
git commit --amend -s -m "[SUS-2594] English version <NEW without v>-eng"
```

Both files must have the same version. If they differ, the executable and the library report
different versions.

## Checks Before Tagging

1. **One commit on top of `<NEW>`:**
   ```bash
   git log --oneline <NEW>..eng/<NEW>
   ```
2. **Build and tests:**
   ```bash
   cmake --build build --config Release --target LGC LGC_Tests
   cd build/LGC_Tests/Release && ./LGC_Tests
   ```
   Tests that compare output text may need the English expected files. Do not change numerical
   expectations.
3. **Results are numerically identical.** Run the same input with the French `<NEW>` executable
   and the English one. Compare the `.res` files: only labels may differ, never numbers.
4. **No French left.** New code from `<NEW>` is not translated by the rebase. Search for it:
   ```bash
   git diff <OLD>..<NEW> --stat -- source/LGC_Core/sources/Writers source/LGC_Core/sources/LGCObjectWriters
   git grep -n -I -E "MESURE|POINTS FIXES|CALAGE|[éèêàçù]" -- source/LGC_Core/sources/Writers source/LGC_Core/sources/LGCObjectWriters
   ```
   Translate any new output strings and fold them in:
   ```bash
   git add <files>
   git commit --amend --no-edit
   ```
5. **Pre-commit passes:** `pre-commit run --from-ref <NEW> --to-ref HEAD`. Fold any fixes in
   with `git commit --amend --no-edit`.
6. **CI rule.** Check that `windows64_release` in `.gitlab-ci.yml` still has the `rules:` for
   `-eng` tags. If `<NEW>` already contains it, nothing to keep there.

## Publishing

```bash
git push -u origin eng/<NEW>
git tag -a <NEW>-eng -m "LGC <NEW without v> English version"
git push origin <NEW>-eng
```

The tag triggers `windows64_release`, which builds the installer. Add the release to
[History](#history) as part of the next port. Do not amend a release that is already tagged.

## History

| English tag | Base tag | Date | Notes |
|-------------|----------|------|-------|
| `v2.11.0-eng` | `v2.11.0` | 2026-07-01 | First English release (8 commits) |
| `v2.11.1-eng` | `v2.11.1` | | Squashed to a single commit |
