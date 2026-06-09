# pyLGC Development Guide

This guide documents the workflow for building the `pyLGC_C` bindings from source and running the Python test suite on Windows.

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

When CMake configures the project, it automatically copies the Python tests and test data (`.lgc2` files) from `source\pyLGC\tests` over to the `build` folder so they sit right next to the freshly compiled DLL. The build's POST_BUILD step also runs the suite once against the build venv, and the test is registered with CTest as `pyLGC_Tests`.

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
