# Contributing to LGC2 code

Thank you for your interest in improving **LGC2**!

Contributions of all kinds are welcome — code, documentation, testing, or design discussions.  
Please read the following guidelines before opening a Pull Request or submitting code.

[[_TOC_]]

---

## Developer Certificate of Origin (DCO)

This project uses the [Developer Certificate of Origin (DCO)](./DCO.txt) version 1.1 to certify contributions.

By contributing, you agree to the following statement (summarized):

> You have the right to submit this work under the open-source license indicated in the project,  
> and you understand that your contribution may be publicly redistributed.

### How to sign your commits

Each commit **must include a `Signed-off-by` line** at the end of the commit message, for example:

```
Signed-off-by: Your Name <your.email@example.com>
```

You can add this automatically by using the `-s` flag when committing:

```
git commit -s
```

Commits without a valid `Signed-off-by` line will not be accepted for merging.

### Future Relicensing Notice

LGC2 is currently distributed under the **GNU General Public License v3.0 or later (GPL-3.0-or-later)**.  
The “or later” clause allows the project owner (CERN) to relicense future versions under a more permissive license if deemed appropriate (for example, Apache-2.0 or another open license).
By signing off your contributions, you acknowledge and permit such future relicensing under the terms of the GPL “or later” provision.

---

## Branching and Workflow

LGC2 uses a lightweight branching model compatible with both GitHub and CERN GitLab.

| Branch | Purpose |
|--------|----------|
| `master` | Stable release branch |
| `appwidevs` | Integration and development branch |
| `feature/*` | Individual feature or fix branches |

Each feature or fix must be developed in its own branch 
Direct commits to `master` or `appwidevs` are not permitted.

Before merging:
- Your branch must be up to date with `appwidevs`.  
- If needed, **rebase** to resolve conflicts.  

### Internal contributors (CERN personnel)
Developers with write access can create branches directly in this repository.  
Changes must be proposed via **Pull Requests (PRs)** and merged only after review and CI validation by a **Maintainer**.

### External contributors
This repository follows the standard **fork → pull request → review → merge** workflow.

### Contribution Steps

1. **Fork** the repository (external contributors).  
2. **Create a branch** from `appwidevs`:  
   ```bash
   git checkout appwidevs
   git checkout -b feature/my-new-function
   ```
3. **Implement changes** and add tests.  
4. **Commit** with DCO sign-off (`git commit -s`).  
5. **Push** your branch and **open a Pull Request** (PR) to `appwidevs`.  
6. A CERN maintainer will review and merge after CI passes.

> Internal contributors can create branches directly in the main repo but must still go through PR review.
> All PRs are reviewed by CERN maintainers. Once approved, changes are merged into the main repository and validated by the internal CI/CD system.

### Commit Best Practices

To maintain a clear history and facilitate reviews, follow these conventions:

- Write descriptive commit messages.  
- Group related changes into single commits.  
- Avoid unrelated formatting or cleanup in functional commits.  
- Ensure each commit builds successfully and passes all tests.

Example message:
```
Add new instrument parser for inclinometer data

Signed-off-by: Jane Doe <jane.doe@cern.ch>
```

---

## Commit Sign-off and Review Process

All PRs undergo automated checks and human review.

1. **Automated Checks**
   - DCO sign-off verification  
   - SPDX license header validation (`reuse lint`)  
   - Build and test execution on multiple platforms  

2. **Human Review**
   - Code quality, maintainability, and style  
   - Correctness and unit test coverage  
   - Compliance with project conventions  

> Only CERN maintainers can merge PRs into protected branches after all checks pass.

---

## Coding and License Requirements

### SPDX and Licensing

Each source file must include SPDX headers to ensure REUSE compliance:

**Example (C++ .cpp):**
```cpp
// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later
```

**Example (C++ header file):**
```cpp
/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
```

### Non-code Files

Non-source files (e.g., `.png`, `.pdf`, `.txt`, `.docx`) must be declared in [`REUSE.toml`](./REUSE.toml).  
License texts for all dependencies are stored under [`LICENSES/`](./LICENSES/).

### REUSE Compliance Check

Install the **REUSE tool**:

```bash
pip install reuse
# or
sudo apt install reuse
```
For more options, see the [official installation guide](https://reuse.readthedocs.io/en/stable/readme.html#installation).

Validate before pushing:
```bash
reuse lint
```

If compliant, you will see:
```
Congratulations! Your project is REUSE compliant.
```

When adding new files:
- Prefer to add SPDX headers directly inside text-based files.  
- If headers are not possible (e.g. for images, binaries, PDFs, test data), update `REUSE.toml` to include them.  
- Always make sure external dependencies are listed in [NOTICE.md](./NOTICE.md) and their license texts are present in `LICENSES/`.

---

## Testing and CI/CD


Automated tests and builds are essential for maintaining reliability across platforms.
All builds and tests must pass before merging.

### Local Testing

Run unit tests locally after building:

```bash
cmake --build . --target LGC_Tests
```

Tests are implemented using **TUT** and executed automatically during CI.

### Continuous Integration

| Type | Platform | Trigger | Description |
|------|-----------|----------|-------------|
| **Internal CI/CD** | CERN GitLab | Merge or push to protected branches | Full builds, packaging, installer creation |
| **Public CI (planned)** | GitHub Actions | PR submission | Basic build and test validation |

The internal CI/CD system ensures safe use of protected CERN resources.  

A public CI workflow (for example, using GitHub Actions) is not yet implemented, but could be a valuable contribution for external collaborators interested in automating build and test validation outside the CERN environment.


#### Current situation
- **Primary development:** GitHub  
- **CI/CD execution:** CERN GitLab mirror  
- Pipelines run on **protected CERN runners**, only triggerable by **CERN maintainers** or **vetted personnel** after merging to protected branches (`appwidevs`, `master`, etc.).  
- External pull requests **do not trigger internal CI/CD** directly — validation occurs after merge.

This ensures:
- Secure use of CERN computing resources  
- Protection of internal credentials and deployment tokens  
- Verified provenance of all executed code  

#### Future plan
The project aims to **gradually migrate CI/CD to GitHub Actions** so that:
- Basic build and test checks run directly on GitHub for *all* contributors  
- Sensitive deployment steps remain protected on CERN GitLab until safely externalized  

This migration is **open to contribution** — if you’re familiar with GitHub Actions, you’re invited to help define or port parts of the existing GitLab pipeline.

### Test Requirements

- All new code must include unit tests.   
- Tests must run cleanly on both Windows and Linux.

---

## Code of Conduct

LGC2 contributors are expected to uphold **CERN’s core values** of:

- **Respect** — Treat all contributors and users with courtesy.  
- **Collaboration** — Share knowledge and support each other’s work.  
- **Openness** — Promote transparency and reproducibility in science.

---

## Contacts and Maintainers


You may reach the maintainers via GitHub discussions or CERN internal email lists.

---

Thank you for helping make **LGC2** a robust, open, and scientifically reliable software platform!  
