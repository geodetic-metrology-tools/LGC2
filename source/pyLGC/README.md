# pyLGC Development Guide

This guide documents the workflow for building the `pyLGC_C` bindings from source and running the Python test suite on Windows.

## 1. Environment Setup

We use `uv` for lightning-fast Python dependency management. Before testing, it is best to set up the virtual environment at the **root of the `LGC2` project**.

Open a terminal at `C:\susoft\LGC2` and run:
```powershell
uv venv
uv pip install numpy scipy pytest
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

When CMake configures the project, it automatically copies the Python tests and test data (`.lgc2` files) from `source\pyLGC\tests` over to the `build` folder so they sit right next to the freshly compiled DLL.

1. Navigate to the tests in the build directory:
   ```powershell
   cd C:\susoft\LGC2\build\pyLGC\tests
   ```
2. Run the pytest suite:
   ```powershell
   uv run pytest tests.py
   ```
