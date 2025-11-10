# Contributing to LGC2 code

Thank you for your interest in improving **LGC2**!

Contributions of all kinds are welcome — code, documentation, testing, or design discussions.  
Please read the following guidelines before opening a Pull Request or submitting code.
This guide applies to contributions made through the GitHub repository, which is now the primary platform for development and collaboration on LGC2.

[[_TOC_]]

---
## GitHub vs GitLab Notice

Historically, the **LGC2** project has been developed within **CERN’s GitLab environment** (`gitlab.cern.ch`).  
The **2.10.0 release** was fully built and maintained using this internal infrastructure.  

The project is now entering a **transition phase** toward a more open and collaborative model.  
Development will progressively move to **GitHub**, with future updates and development work published under the `appwidevs` branch (check this branch for status update)  
This migration aims to make the project’s source code, issue tracking, and documentation more accessible to the broader scientific and open-source communities.  
It also creates new opportunities for **community contributors** to participate in development, testing, and documentation improvements.

For the time being, the **continuous integration and deployment (CI/CD)** processes continue to run on CERN’s GitLab infrastructure.
In a later phase, the CI/CD system will also be **migrated to GitHub**, providing a unified, open workflow for building, testing, and packaging the software.  
This migration will represent a **valuable entry point for contributors**.

> During this transition, both GitLab and GitHub repositories will remain synchronized,  
> and development activity will increasingly focus on GitHub.  
> For contribution procedures, please continue reading below.

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

LGC2 is currently distributed under the **GNU General Public License v3.0 (GPL-3.0)**.

CERN, as the project steward, may — if all dependencies and legal conditions permit — **relicense or dual-license future versions of the project under another OSI-approved open-source license** (for example, **LGPL-3.0**, **Apache-2.0**, or **MIT**) to facilitate wider scientific, academic, and industrial reuse.  

By signing your commits in accordance with the [Developer Certificate of Origin (DCO)](./DCO.txt), you explicitly acknowledge and authorize CERN to include your contributions in such future relicensed or dual-licensed versions, **provided that any new license remains OSI-approved and preserves open access to the source code.**

Any future relicensing will remain within the framework of recognized open-source principles and will never introduce proprietary or restrictive terms.

---

## Branching and Workflow

Members of the LGC team (maintainers) can push branches directly to the main repository, while community contributors are encouraged to work through forks and pull requests.
All changes go through review before being merged.

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


### Contribution Steps

1. **Fork** the repository (community contributors).  
2. **Create a branch** from `appwidevs`:  
   ```bash
   git checkout appwidevs
   git checkout -b feature/my-new-function
   ```
3. **Implement changes** and add tests.  
4. **Commit** with DCO sign-off (`git commit -s`).  
5. Submit a **Pull Request (PR)** to `appwidevs`.  
6. Your PR will be reviewed and validated before merging.

> Maintainers can create branches directly in the main repo but must still go through PR review.
> All PRs are reviewed by LGC maintainers. Once approved, changes are merged into the main repository and validated by the internal CI/CD system.

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

> Only **LGC Maintainers** can merge Pull Requests into protected branches once all checks have passed.  
> See the [Testing and CI/CD](#testing-and-cicd) section for details about the current pipeline configuration.

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

#### Current Situation
- **Primary development:** GitHub  
- **CI/CD execution:** CERN GitLab mirror  
- Pipelines run on **protected CERN runners** and can only be triggered by **LGC Maintainers with CERN credentials** after merges to protected branches (`appwidevs`, `master`, etc.).  
  This ensures secure use of CERN resources and verified provenance of all executed code.  
- Pull requests from forks **do not yet trigger internal CI/CD** — validation occurs after merging.

#### Future Plan
The project aims to **gradually migrate CI/CD to GitHub Actions**, enabling build and test validation directly on GitHub for all contributors.  
Sensitive deployment steps will remain on CERN GitLab until safely externalized.  
Community participation in this migration — particularly from contributors familiar with GitHub Actions — is warmly welcomed.

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

You may reach the maintainers via GitHub discussions or email lists.

---

Thank you for helping make **LGC2** a robust, open, and scientifically reliable software platform!  
