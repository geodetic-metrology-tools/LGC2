[![pipeline status](https://gitlab.cern.ch/apc/susofts/processing/LGC2/badges/master/pipeline.svg)](https://gitlab.cern.ch/apc/susofts/processing/LGC2/commits/master)

# LGC2

**LGC2** is a command-line software originally developed at **CERN** for precise survey and alignment computations.  
It applies the **least-squares adjustment method** to determine the spatial positions of measured points and associated statistics from geodetic or metrological observations.

[[_TOC_]]

---

## Purpose

LGC2 is a **major evolution** of the original *LGC* (C++ version 1), extending its capabilities to meet the needs of the **HIE-ISOLDE** project and future large-scale CERN installations.  
The new architecture and observation model are documented in [EDMS 1465539](https://edms.cern.ch/document/1465539).


Key improvements:

- Introduction of **complex (adjustable) objects** such as planes, lines, and coordinate transformations.  
- Reformulated **mathematical observation models**, enabling new measurement types.  
- Support for **new observation types and instruments**, such as wire positioning sensors and inclinometers.  
- Implementation of a **hierarchical local-frame system** for flexible geometry definitions.  
- A **newer input file format** for improved readability and extensibility.

---

## Download
The latest compiled versions of **LGC2** are currently available through CERN’s internal repositories.

- **CERN users:** Access the official releases on [CERN GitLab Releases](https://gitlab.cern.ch/apc/susofts/processing/LGC2/-/releases).  
- **External collaborators:** Temporary public builds are available for download [here](https://cernbox.cern.ch/s/CXj9Am33jGah5a8).

Both **Windows** and **Linux** versions are provided at this link.

> A permanent public distribution platform (e.g. GitHub Releases) will be defined once the open-source publication process is finalized.	

---

## Documentation

### User Guides

Comprehensive user documentation is available in two forms:

- **[CERN Internal User Guide](https://confluence.cern.ch/display/SUS/LGC2+User+Guide)** — maintained within CERN’s Confluence environment.  
- **[Public User Guide](https://lgc2.docs.cern.ch/)** — hosted externally for the open-source community.

The long-term goal is to maintain the **public user guide** as the single, authoritative source of documentation.  
As of September 2025, the migration is in progress, and most content is being consolidated into the public **MkDocs-based** documentation site, which will also be hosted in the same GitHub organization.

### Doxygen Developer Documentation

The **Doxygen documentation** is intended for developers and contributors.  
Follow the [Build Instructions](#build-instructions) to set up your environment, then build the `doc` target to generate the documentation.

You will need:
- [**Doxygen**](https://www.stack.nl/~dimitri/doxygen/download.html#srcbin)
- [**GraphViz**](http://www.graphviz.org/download/#executable-packages)

Once built, open the following file as the main entry point for the developer documentation:
```bash
build/html/index.html
```
> The Doxygen coverage is currently partial. There are no immediate plans to maintain or complete it.
> This could be an interesting area for external contributions aiming to improve the understanding of the LGC2 codebase.

### Additional Documentation

Additional materials can be found in the [`doc/`](./doc) directory.

> The mathematical model documentation (PDF format, last updated in 2020) is no longer actively maintained.  
> It remains largely accurate but is scheduled for integration into the new public user guide.

---

## Build instructions
These instructions describe how to build and package **LGC2** from source on Windows and Linux.  
They cover both internal CERN environments (with access to private submodules) and external open-source setups.

> In the future, only build instructions for the public repository should be provided.

### Overview

LGC2 can be built on both **Windows** and **Linux** platforms.  
It is primarily developed with **CMake** and supports multiple build environments, including **Visual Studio**, **Eclipse**, and standard **Unix Makefiles**.

CERN internal developers can refer to the detailed Confluence guide for the full setup and environment configuration:  
**[Getting Started with C++ for Survey Applications (CERN internal)](https://confluence.cern.ch/pages/viewpage.action?pageId=22153013)**

External contributors can follow the summarized instructions below.

### Prerequisites

The following tools and dependencies are required:

| Component | Version / Example | Purpose |
|------------|------------------|----------|
| **C++ Compiler** | C++14-compliant (MSVC v142, GCC ≥ 7.0, Clang ≥ 5.0) | Core compilation |
| **CMake** | [≥ 3.10 (recommended 3.20+)](https://cmake.org/download/) | Project configuration |
| **NSIS** | [3.05](https://nsis.sourceforge.io/Main_Page) | Windows installer generation |
| **Eigen** | [3.4.0](https://eigen.tuxfamily.org/) | Matrix and numerical computations |
| **TUT** | [2016-12-19](https://mrzechonek.github.io/tut-framework/) | Unit testing framework |
| **Git** | [Latest stable](https://gitforwindows.org/) | Source control and submodules |
| **Doxygen** | [≥ 1.8.18](https://www.doxygen.nl/download.html) | Developer documentation generation (optional) |
| **GraphViz** | [≥ 2.38](https://graphviz.org/download/) | Visualization in Doxygen (optional) |

#### Windows Environment (External Setup)

If you are developing on **Windows**, follow these summarized steps adapted from the CERN internal setup:

1. **Install Visual Studio 2019 or later**  
   - Launch Visual Studio Installer → *Modify Installation* → *Individual Components* tab.  
   - Ensure the following components are selected:  
     - *C++ 2019 Redistributable Update*  
     - *MSVC v142 - VS 2019 C++ x64/x86 build tools*  
     - *C++ core features*  
     - *Windows 10 SDK (10.0.16299.0)* or later  

2. **Install Git for Windows**   
   - Ensure the installer adds Git to your system PATH.

3. **Install CMake**  
   - Download from [https://cmake.org/download/](https://cmake.org/download/).  
   - During installation, select *Add CMake to PATH for all users*.  

4. **Install NSIS (optional, for packaging)**  
   - Download version 3.05 from [https://nsis.sourceforge.io/Download](https://nsis.sourceforge.io/Download).

5. **Install Eigen and TUT**  
   - Clone Eigen:  
     ```bash
     git clone https://gitlab.com/libeigen/eigen.git
     cd eigen && git checkout 3.4.0
     ```  
   - Clone TUT:  
     ```bash
     git clone https://github.com/mrzechonek/tut-framework.git
     cd tut-framework && git checkout 2016-12-19
     ```  
   - Place both in a directory accessible to your project (e.g. `C:\dev\ext\`).

6. **Install Doxygen and GraphViz (optional)**  
   - [Doxygen](https://www.doxygen.nl/download.html)  
   - [GraphViz](https://graphviz.org/download/) (add `bin/` directory to PATH)

> This setup represents the standard CERN configuration but can be adapted as needed.


#### Linux Environment

For Linux systems, LGC2 can be built with standard development tools:

```bash
sudo apt install build-essential cmake git doxygen graphviz
```

If packaging or testing requires it:

```bash
sudo apt install nsis
```

All other dependencies (e.g., Eigen, TUT, tree.hh) are automatically fetched by CMake via `FetchContent`.

> On CentOS 7 (CC7), install `devtoolset-7` or later to enable C++14:  
> `sudo yum install devtoolset-7 && scl enable devtoolset-7 bash`


### Submodules and External Dependencies

LGC2 depends on two key submodules providing shared functionality and core routines:

| Submodule | Purpose | Repository (Internal) | Repository (Public) |
|------------|----------|----------------------|---------------------|
| **SurveyLib** | Core geodetic and adjustment algorithms shared across SU software | `https://gitlab.cern.ch/apc/susofts/libraries/SurveyLib` | `https://github.com/geodetic-metrology-tools/SurveyLib` |
| **SUSoftCMakeCommon** | Common CMake configuration, packaging setup, and compiler options | `https://gitlab.cern.ch/apc/susofts/shared/SUSoftCMakeCommon` | `https://github.com/geodetic-metrology-tools/SUSoftCMakeCommon` |

LGC2 also uses the following third-party libraries fetched automatically via CMake’s [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html):

- **[tree.hh](https://github.com/kpeeters/tree.hh)** – A lightweight C++ header-only tree container used for hierarchical data.  
  _Licensed under the [GNU General Public License v3.0 (GPL-3.0)](https://www.gnu.org/licenses/gpl-3.0.html)._

These dependencies are cloned automatically during configuration. 

You do **not** need to install them manually.

> `tree.hh` is licensed under GPL-3.0, a strong copyleft license. Distributing binaries that include it requires providing full source code and license text.


### Clone and Configure the Repository

#### Clone the repository

- **CERN internal clone:**
  ```bash
  git clone https://gitlab.cern.ch/apc/susofts/processing/LGC2.git
  ```

- **Public GitHub clone:**
  ```bash
  git clone https://github.com/geodetic-metrology-tools/LGC2.git
  ```

Then:
```bash
cd LGC2
```

#### Submodule Setup


- **Internal (CERN) users**
  ```bash
  git submodule update --init --recursive
  ```

- **External (public GitHub) users**
  **SurveyLib** and **SUSoftCMakeCommon** submodule are publicly available.  
  
  ```bash
	# Initialize submodules without fetching content yet
	git submodule init
	
	# Fix URLs for public repositories
	git config submodule.lib/SurveyLib.url https://github.com/geodetic-metrology-tools/SurveyLib.git
	git config submodule.lib/SUSoftCMakeCommon.url https://github.com/geodetic-metrology-tools/SUSoftCMakeCommon.git
	
	# Fetch content from the corrected public URLs
	git submodule update --remote lib/SurveyLib
	git submodule update --remote lib/SUSoftCMakeCommon
  ```

> Do **not** run `--recursive` before fixing the URL; the default CERN paths will fail for external users.

##### External Dependency Configuration

**SUSoftCMakeCommon** provides:

- Default compiler and build flags
- Doxygen setup options
- Installer and packaging configuration (`create_default_installer`)
- Helper functions for DLL copy and installer generation

External developers can adjust `"lib/SUSoftCMakeCommon/ext_libs.txt"` to define paths for locally installed dependencies:
```cmake
set(EXT_LIB_PATH "C:/dev/ext")
set(EIGEN_INCLUDE_PATH "${EXT_LIB_PATH}/eigen")
set(TUT_INCLUDE_PATH "${EXT_LIB_PATH}/tut-framework")
```

These paths can also be overridden at configuration time:
```bash
cmake -DEXT_LIBS_TXT_PATH="C:/path/to/custom/ext_libs.txt" ../source
```

### Build the Project

#### Generate with CMake
```bash
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -A x64 ../source
```

You may specify a custom dependency file if needed:
```bash
cmake -G "Visual Studio 16 2019" -A x64 -DEXT_LIBS_TXT_PATH="C:/dev/ext_libs.txt" ../source
```

#### Build
```bash
cmake --build . --target ALL_BUILD
```

#### Optional: Create Installer (Windows only)
```bash
cmake --build . --target PACKAGE
```

#### Run Tests
```bash
cmake --build . --target LGC_Tests
```
> The tests are automatically executed on GitLab CI for each contribution.  
> Setting up GitHub Actions for CI testing would be a valuable contribution.

#### Quick Build Example (Linux / Public)
```bash
git clone https://github.com/geodetic-metrology-tools/LGC2.git
cd LGC2
git submodule init lib/SurveyLib
git config submodule.lib/SurveyLib.url https://github.com/geodetic-metrology-tools/SurveyLib.git
git submodule update --remote lib/SurveyLib
cmake -B build -S source
cmake --build build
```

### Project Targets

CMake generates the following main targets:

| Target | Description |
|---------|-------------|
| `ALL_BUILD` | Builds all components except documentation |
| `PACKAGE` | Creates Windows installer (requires NSIS) |
| `ZERO_CHECK` | Re-runs CMake to update build files |
| `RUN_TESTS` | Executes all unit tests |
| `doc` | Builds the Doxygen developer documentation |
| `LGC` | Main command-line executable |
| `LGC_Core` | Static computation library |
| `LGC_Tests` | Unit test suite |

### Packaging and Distribution

To create an installation package, build the `PACKAGE` target.  
The resulting installer is generated in the `build/` directory.

It is recommended to test the installer in a clean or virtual environment before distribution.

---

## Contributing

All contributions are **warmly welcomed** — whether from CERN personnel, collaborators from other institutes, or independent developers interested in large-scale or geodetic metrology software.

### How to Report Issues

- **CERN contributors:**  
  Report or track issues on the [CERN Jira board](https://its.cern.ch/jira/browse/SUS).  
  All internal developments and bug reports must have a corresponding Jira issue.

- **External contributors:**  
  Open a [GitHub Issue](https://github.com/geodetic-metrology-tools/LGC2/issues) to:  
  - Report bugs  
  - Suggest improvements or new features  
  - Discuss documentation or CI/CD integration  

> Maintainers will ensure smooth coordination between the internal and public issue-tracking systems.
> Future plans include publishing a consolidated project roadmap, milestone tracker, and CI/CD activity dashboard.

### How to Contribute Code

This repository follows the standard **fork → pull request → review → merge** workflow.
See the [`CONTRIBUTING.md`](./CONTRIBUTING.md) for details about how to contribute for **CERN personnel** and **externals**.

---

## Licensing

LGC2 is licensed under the [GNU General Public License v3.0 or later (GPL-3.0-or-later)](LICENSE).

Documentation, metadata, and non-code assets (e.g., configuration or documentation files) are licensed under the [Creative Commons Attribution 4.0 International (CC-BY-4.0)](https://creativecommons.org/licenses/by/4.0/).

This distinction and file classification are defined in [`REUSE.toml`](./REUSE.toml).

### Third-Party Dependencies

| Library | License | Source |
|----------|----------|--------|
| Eigen | MPL-2.0 | [https://gitlab.com/libeigen/eigen](https://gitlab.com/libeigen/eigen) |
| TUT | BSD 2-Clause | [https://github.com/mrzechonek/tut-framework](https://github.com/mrzechonek/tut-framework) |
| tree.hh | GPL-3.0-or-later | [https://github.com/kpeeters/tree.hh](https://github.com/kpeeters/tree.hh) |
| SurveyLib | GPL-3.0-or-later | [https://github.com/geodetic-metrology-tools/SurveyLib](https://github.com/geodetic-metrology-tools/SurveyLib) |
| SUSoftCMakeCommon | GPL-3.0-or-later | [https://github.com/geodetic-metrology-tools/SUSoftCMakeCommon](https://github.com/geodetic-metrology-tools/SUSoftCMakeCommon) |

All license texts are available in the [`LICENSES/`](./LICENSES/) directory.  
A summary of dependencies and license terms is included in [`NOTICE.md`](./NOTICE.md).

### License Compliance (REUSE)

This project follows the [REUSE Specification](https://reuse.software/) to ensure proper copyright and license attribution.

To verify compliance, install the **REUSE tool** and run:

```bash
reuse lint
```

If successful, you’ll see:
```
Congratulations! Your project is REUSE compliant.
```

When adding new files:
- Add SPDX headers directly to text-based files where possible.  
- If headers are not possible (e.g., binaries, PDFs), list them in [`REUSE.toml`](./REUSE.toml).  
- Add any new third-party licenses to the [`LICENSES/`](./LICENSES/) folder.

---

Thank you for contributing to this project and helping us make **geodetic metrology software** open, reliable, and accessible to the wider community!
