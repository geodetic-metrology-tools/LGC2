[![pipeline status](https://gitlab.cern.ch/apc/susofts/processing/LGC2/badges/master/pipeline.svg)](https://gitlab.cern.ch/apc/susofts/processing/LGC2/commits/master)

LGC2
=========

LGC2 is a command line CERN survey software that uses the least squares method to determine the real position of measured points.

#### Table of Content ####

[Purpose](#purpose)

[Download](#download)

[Documentation](#documentation)
- [User guide](#user-guide)
- [Doxygen](#doxygen)
- [Other](#other)

[Build instructions](#build-instructions)
- [Requirements](#requirements)
- [Generate project](#generate-project)
- [Build](#build)
- [Tests](#tests)

[Contribute](#contribute)
- [Jira](#jira)
- [Pull requests](#pull-requests)
- [Automatic tests](#automatic-tests)

Purpose
-------

A major update of the LGC (C++ version 1), provides extended functionalities required for the HIE-Isolde project and more. The new features are documented in the EDMS document number: 1465539.

Main shifts:
- Complex (adjustable) objects introduced (planes, lines, transformations).
- Mathematical observations models described in terms of these complex objects and therefore have been reformulated, new measurement types introduced.
- Introduced camera instrument and camera measurements.
- Hierarchy of local frames has been added.
- New input file format.


Download
--------

You can download the last version of LGC2 in the [Releases](https://gitlab.cern.ch/apc/susofts/processing/LGC2/-/releases)

Documentation
-------------

### User guide ###

You can find all the user documentation here:
- [user guide](https://confluence.cern.ch/display/SUS/LGC2+User+Guide)

#### MkDocs 

The plan is to move the full documentation (on-going)  [MkDocs](https://www.mkdocs.org/).

**Build and serve the MkDocs documentation**

With `Python` available on your  machine, to serve the documentation locally:

```bash
# Navigate to the root of the project (where mkdocs.yml is)
cd LGC2/doc

# (optional) Create and activate a virtual environment
python3 -m venv .venv
source .venv/bin/activate

# Install MkDocs and the Material theme
pip install mkdocs mkdocs-material

# Serve the docs locally
mkdocs serve
```

Then open [http://127.0.0.1:8000](http://127.0.0.1:8000) in your browser.

To build the static site:

```bash
mkdocs build
```

The resulting HTML will be in the `site/` folder.

#### CI/CD Integration

> ⚠️ The current documentation system is not yet integrated into the project’s CI/CD pipelines. Work is still required to:
> - Automate the building and deployment of the MkDocs site
> - Possibly publish to CERN GitLab Pages or another hosting service
> - Include checks or preview builds in merge requests

### Doxygen ###

The Doxygen documentation is meant for developers only. Follow the [Build instructions](#build-instructions) to set up your projects. Then you can build the `doc` target to create the Doxygen documentation. You will need [Doxygen](https://www.stack.nl/~dimitri/doxygen/download.html#srcbin) and [GraphViz](http://www.graphviz.org/download/#executable-packages) installed and configured.

Once built, you can open the file `build/html/index.html` as an entry point to the documentation.

> ⚠️ Doxygen code documentation is partial and is not thoroughly implemented evywhere. To be decided if it should be generalized or abandoned. 


### Other ###

You can find further documentation in the folder [doc](./doc).

> ⚠️ The mathematical model PDF is no longer actively maintained (last update: 2020). While still largely accurate, it is pending review and intended to be integrated directly into the new user manual in MkDocs format.

Build instructions
------------------

Before starting, you can have a look at the documentation about [Getting started with C++](https://confluence.cern.ch/pages/viewpage.action?pageId=22153013) for the CERN survey applications.

### Requirements ###

LGC2 can be built on Windows or Linux. To do so, you need at least:
- a C++14 compiler
- CMake 3.6+
- NSIS
- Eigen
- TUT

For Windows, you can follow the steps in the aforementioned [Getting started with C++](https://confluence.cern.ch/pages/viewpage.action?pageId=22153013) documentation.

For Linux, you have an example of the needed steps in the dockerfiles of the [sus_ci_cppworker](https://gitlab.cern.ch/apc/common/docker-image-susoft-cpp) project (the Docker image used to automatically run the tests on GitLab-CI).
Note that the `devtoolset` trick is only necessary on the CC7 (Cern CentOS 7) as it doesn't provide a C++14 compiler by default.

### External Dependencies ###

LGC2 uses the following third-party libraries fetched automatically during the CMake configuration phase via [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html):

- **[tree.hh](https://github.com/kpeeters/tree.hh)** – A lightweight C++ header-only tree container used for hierarchical data.  
  _Licensed under the [GNU General Public License v3.0 (GPL-3.0)](https://www.gnu.org/licenses/gpl-3.0.html)._

These dependencies are cloned automatically during the build configuration step. You do **not** need to install them manually.

> `tree.hh` is licensed under GPL-3.0, a strong copyleft license. If you distribute binaries that include `tree.hh`, your project must comply with the GPL-3.0—this typically means making the full corresponding source code and license available to users.


### Generate project ###

We use CMake to generate projects, thus it is possible to generate projects for MSVC, Eclipse, or simple Unix makefiles. See the [CMake Generators documentation](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) page.

To generate the project, you need first to create a subdirectory named `build/`, and then run CMake inside:

```bash
# first we download all the submodules
$ git submodule update --init
# then we generate the project
$ mkdir build && cd build/
$ cmake -G "Visual Studio 16 2019" -A x64 ../source # Use another generator here if you wish
```
In order to use a custom ext_libs.txt file defining the dependencies, please use:

```bash
$ cmake -G "Visual Studio 16 2019" -A x64 -DEXT_LIBS_TXT_PATH="C:/susoft/SUSoftCMakeCommon/ext_libs.txt" ../source # Use another file defining the dependencies
```

### Build ###

Once generated, you can open your project in the `build/` subfolder. If you use MSVC, you can open the file `build/LGC2.sln`.

you can see that CMake has generated several targets, among others:
- `ALL_BUILD` builds all except the doxygen documentation
- `PACKAGE` builds the Windows installer
- `ZERO_CHECK` reruns CMake and automatically updates your project
- `RUN_TESTS` runs all tests provided they were build before
- `doc` builds the Doxygen documentation
- `LGC` builds the command line executable (default project in MSVC)
- `LGC_Core` builds the static library that handles all LGC computations
- `LGC_Tests` builds the tests

### Tests ###

To build the tests, build the target `LGC_Tests` and run it. We Use TUT to generate unit tests. Note that the tests are automatically performed on Gitlab-CI for each contribution. You can see the results in the [CI page](https://gitlab.cern.ch/apc/susofts/processing/LGC2/pipelines).

###  ###

Contribute
----------

LGC2 is a private CERN repository, thus it doesn't accept contributions from outside CERN.

To report an issue (bug, or feature request), follow the [Jira](#jira) subsection. For development, please read on.

### Jira ###

Any request, bug or development should have a Jira issue. You can create an issue on the [dedicated Jira board](https://its.cern.ch/jira/browse/SUS). This is mandatory for both the users and the developers.

### Pull requests ###

The most up-to-date stable branch is `master`. The beta branch is `appwidevs`. As stable branches, you **must not** commit directly in them. You need to create a specific branch for your on-going development and commit there. As we use CMake, if you add a file, don't forget to add it in one of the `CMakeFile.txt`!

Once you have finished your work, you should create a Pull Request (PR, or Merge Request) from your branch to `appwidevs`. The description of your PR should include a link to the corresponding task in Jira.

Once your PR has been reviewed by another developer and accepted, it can be merged into `appwidevs`. Note that, for the sake of a nice Git history, your branch needs to be up to date with `appwidevs`. If it is not the case, you will have to rebase, either automatically from GitLab if there are no conflicts, or manually otherwise.

### Automatic tests ###

Automatic tests are performed each time you push a commit. These tests include compilation of `ALL_BUILD` target, and running the `LGC_Tests` target, all on Linux 64 bits, Windows 32 and 64 bits. If the tests don't pass, your PR will not be merged.

For each release, when `master` is updated, GitLab-CI will automatically build the installers.

### Third-Party Libraries ### 

| Library   | Purpose                          | License      |
|-----------|----------------------------------|--------------|
| `tree.hh` | Lightweight tree data structure  | [GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html) |
