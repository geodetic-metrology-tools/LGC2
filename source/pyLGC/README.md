# pyLGC Development Guide

This guide documents the workflow for building the `pyLGC_C` bindings from source and running the Python test suite on Windows and Linux.

## Platform support

| Platform | Status |
|---|---|
| Windows (x64)   | Supported — used in CI and by maintainers day-to-day. |
| Linux (x86_64)  | Supported — built and tested in CI. |
| macOS           | Not officially supported and never tested. The CMake + C++ build is plausibly portable and the Python wrapper would only need a small change to load `libpyLGC_C.dylib` instead of `libpyLGC_C.so` (see the comment in [`pyLGC.py`](pyLGC.py) near the `_dll_name` line) |

## 1. Environment Setup

**You do not need to install anything yourself to run the tests through CMake / CTest.** When CMake configures the project, it automatically provisions an isolated virtual environment at `<build>/pyLGC/venv` and installs the dependencies listed in [`tests/requirements.txt`](tests/requirements.txt). This is what CI runs, and it's what you get out of the box on a fresh machine.

The CMake-managed venv is wiped by `git clean -fdx build/`; it is rebuilt the next time you reconfigure. If you ever want to force a clean reinstall (e.g. you suspect a corrupted package), just delete `<build>/pyLGC/venv/` and reconfigure.

### Optional: working outside the build with `uv`

If you'd prefer your own environment for hacking on tests or running the example scripts (which also need `scipy`), `uv` is the fastest way:
```powershell
uv venv
uv pip install -r source\pyLGC\tests\requirements.txt scipy
```

**Note on Activation:**
Because you are using `uv run ...` for your commands, `uv` automatically detects the `.venv` folder in the directory tree and uses it on the fly. You do *not* strictly need to activate it manually!
However, if you want to run plain `pytest` or `python` without the `uv run` prefix, you must activate the environment first:
```powershell
.venv\Scripts\activate
```

## 2. Building the C-API (`pyLGC_C`)

The Python wrappers rely on a DLL built from the legacy C++ core.

1. From the `LGC2` root directory, create a `build` folder and navigate into it:
   ```powershell
   mkdir build
   cd build
   ```
2. Generate the build files using CMake (requires version 3.44+):
   ```powershell
   cmake ../source
   ```

### Option A: Building via Visual Studio (GUI)
1. Double click the newly generated `LGC.sln` file in the `build` directory to open it in Visual Studio.
2. In the **Solution Explorer**, find the `pyLGC_C` target.
3. Right-click `pyLGC_C` and select **Build**.

### Option B: Building via CLI (Alternative)
You can completely skip opening Visual Studio by using CMake's build command directly from your terminal (still inside the `build` folder):
```powershell
cmake --build . --config Release --target pyLGC_C
```
*(You can change `--config Release` to `--config Debug` if you need to attach a debugger).*

## 3. Running the Tests

When CMake configures the project, it automatically copies the Python tests and test data (`.lgc2` files) from `source\pyLGC\tests` over to the `build` folder so they sit right next to the freshly compiled DLL. The build's POST_BUILD step also runs the suite once against the build venv, **and a pytest failure fails the build** — just like a compile error. The same suite is also registered with CTest as `pyLGC_Tests`.

To run the suite manually after a build, the simplest path is CTest from the build directory:
```powershell
ctest --test-dir build -R pyLGC_Tests --output-on-failure
```

Or invoke pytest directly using the build venv's Python so you don't need any activation step:
```powershell
build\pyLGC\venv\Scripts\python -m pytest build\pyLGC\tests\tests.py -v
```

If you set up your own `uv` environment (see section 1), the alternative is:
```powershell
cd C:\susoft\LGC2\build\pyLGC\tests
uv run pytest tests.py
```

## 4. Using the pre-built release artifact

If you do not want to build from source, pre-built artifacts are published on [CERN GitLab Releases](https://gitlab.cern.ch/apc/susofts/processing/LGC2/-/releases) by the CI pipeline for each release tag.

Each release provides a flat bundle containing:

| File | Windows | Linux |
|------|---------|-------|
| Native library | `pyLGC_C.dll` | `libpyLGC_C.so` |
| Python wrapper | `pyLGC.py` | `pyLGC.py` |
| Dependencies | `requirements.txt` | `requirements.txt` |

### Setup

1. Download and place all three files in the same directory.
2. Install the Python dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Make sure `pyLGC.py` and the native library are on your Python path (or in the working directory):
   ```python
   import sys
   sys.path.insert(0, "/path/to/pyLGC/bundle")
   import pyLGC
   ```

### Minimal usage example

```python
import pyLGC

ev = pyLGC.Evaluator("myproject.lgc2")
ev.evaluate()

dims = ev.getProblemDimensions()
print(f"Unknowns: {dims.UIndex}, Observations: {dims.OIndex}")

params = ev.getEstimatedParameters()
print("Estimated parameters:", params)

# Access a named point or frame
point = ev.getPoint("MY_POINT")
print(point.getName(), point.getEstVector())
```

For full API documentation and extended examples, refer to the [pyLGC manual](doc/).

## 5. Using from C/C++ (or other languages) _(untested)_

The bundle also ships `pyLGC_C.h` — the plain C header. You can link against the DLL/SO directly from any language with C FFI, without Python.

**Minimal C example:**
```c
#include "pyLGC_C.h"
#include <stdio.h>

int main(void) {
    LGCEvaluator ev = lgcEvaluatorCreate("myproject.lgc2");
    if (!ev) { fprintf(stderr, "%s\n", lgcGetLastError()); return 1; }

    lgcEvaluatorEvaluate(ev);

    int u, e, o, c;
    lgcEvaluatorGetProblemDimensions(ev, &u, &e, &o, &c);
    printf("Unknowns: %d, Observations: %d\n", u, o);

    double *params = NULL; int n = 0;
    lgcEvaluatorGetEstimatedParameters(ev, &params, &n);
    lgcFreeDoubleArray(params);

    lgcEvaluatorDestroy(ev);
    return 0;
}
```

**Memory management:** any `double**` or `int**` output parameter is heap-allocated by the library — free it with `lgcFreeDoubleArray` / `lgcFreeIntArray` after use.

The same header works from MATLAB (`loadlibrary`), Julia (`ccall`), R, or any other runtime that can load a shared library.

> Integration beyond Python has not been fully tested by the maintainers. If you use pyLGC from another language, feedback and contributions are welcome via [GitHub Issues](https://github.com/geodetic-metrology-tools/LGC2/issues).

**R — loading the library (tested):**
```r
dyn.load("pyLGC_C.dll")  # or "libpyLGC_C.so" on Linux
getNativeSymbolInfo("lgcEvaluatorCreate")  # symbol resolves correctly
dyn.unload("pyLGC_C.dll")
```

> R's base FFI (`.C()`) cannot store or pass back opaque `void*` handles, which this API uses for `LGCEvaluator` and `LGCAdjObj`. Full integration requires `Rcpp` or a dedicated C wrapper.
