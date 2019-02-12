[![pipeline status](https://gitlab.cern.ch/apc/susofts/processing/LGC2/badges/master/pipeline.svg)](https://gitlab.cern.ch/apc/susofts/processing/LGC2/commits/master)

SurveyLib
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

You can download the last version of LGC2 installer here:
- Linux (64 bits) - executable only: [LGC](https://gitlab.cern.ch/apc/susofts/processing/LGC2/-/jobs/artifacts/master/raw/LGC?job=linux_release)
- Windows (64 bits): [LGCInstaller-2.02.00-win64.exe](https://gitlab.cern.ch/apc/susofts/processing/LGC2/-/jobs/artifacts/master/raw/LGCInstaller-2.02.00-win64.exe?job=windows64_release)
- Windows (32 bits): [LGCInstaller-2.02.00-win32.exe](https://gitlab.cern.ch/apc/susofts/processing/LGC2/-/jobs/artifacts/master/raw/LGCInstaller-2.02.00-win32.exe?job=windows32_release)

Documentation
-------------

### User guide ###

You can find all the user documentation here:
- [user guide](https://readthedocs.web.cern.ch/display/SUS/LGC2+User+Guide)
- [release notes](https://readthedocs.web.cern.ch/display/SUS/LGC+v2+Release+Notes)

### Doxygen ###

The Doxygen documentation is meant for developers only. Follow the [Build instructions](#build-instructions) to set up your projects. Then you can build the `doc` target to create the Doxygen documentation. You will need [Doxygen](https://www.stack.nl/~dimitri/doxygen/download.html#srcbin) and [GraphViz](http://www.graphviz.org/download/#executable-packages) installed and configured.

Once built, you can open the file `build/html/index.html` as an entry point to the documentation.

### Other ###

You can find further documentation in the folder [doc](./doc). You can also find some [notes on the code](https://readthedocs.web.cern.ch/display/SUS/Notes+on+the+code).

Build instructions
------------------

Before starting, you can have a look at the documentation about [Getting started with C++](https://readthedocs.web.cern.ch/pages/viewpage.action?pageId=22153013) for the CERN survey applications.

### Requirements ###

LGC2 can be built on Windows or Linux. To do so, you need at least:
- a C++14 compiler
- CMake 3.6+
- NSIS
- Eigen
- TUT
- TClap
- Boost

For Windows, you can follow the steps in the aforementioned [Getting started with C++](https://readthedocs.web.cern.ch/pages/viewpage.action?pageId=22153013) documentation.

For Linux, you have an example of the needed steps in the [Dockerfile](https://gitlab.cern.ch/apc/susofts/shared/sus_ci_cppworker/blob/master/Dockerfile) of the [sus_ci_cppworker](https://gitlab.cern.ch/apc/susofts/shared/sus_ci_cppworker) project (the Docker image used to automatically run the tests on GitLab-CI).
Note that the `devtoolset` trick is only necessary on the CC7 (Cern CentOS 7) as it doesn't provide a C++14 compiler by default.

### Generate project ###

We use CMake to generate projects, thus it is possible to generate projects for MSVC, Eclipse, or simple Unix makefiles. See the [CMake Generators documentation](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html) page.

To generate the project, you need first to create a subdirectory named `build/`, and then run CMake inside:

```bash
# first we download the SurveyLib as a submodule
$ git submodule update --init
# then we generate the project
$ mkdir build && cd build/
$ cmake -G "Visual Studio 15 2017 Win64" ../source # Use another generator here if you wish
```

### Build ###

Once generated, you can open your project in the `build/` subfolder. If you use MSVC, you can open the file `build/LGC2.sln`.

you can see that CMake has generated several targets, among others:
- `ALL_BUILD` builds all except the doxygen documentation
- `ZERO_CHECK` reruns CMake and automatically updates your project
- `PACKAGE` builds the Windows installer
- `doc` builds the Doxygen documentation
- `SpatialObjDLL` builds the SurveyLib dynamic library
- `LGC_Tests` builds the tests
- `LGC` builds the command line executable (default project in MSVC)

### Tests ###

To build the tests, build the target `LGC_Tests` and run it. We Use TUT to generate unit tests. Note that the tests are automatically performed on Gitlab-CI for each contribution. You can see the results in the [CI page](https://gitlab.cern.ch/apc/susofts/processing/LGC2/pipelines).

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
