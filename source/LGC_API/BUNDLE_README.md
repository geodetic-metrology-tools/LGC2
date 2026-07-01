# LGC2 Monitor library - bundle

Self-contained bundle for BOTH the Python monitoring API and direct C/C++ use of
the monitoring library. The native library exports a single, stable flat **C ABI**
(the `moni*` functions); the C++ `Monitor` class and the Python wrapper are thin
layers on top of it, so only C types ever cross the library boundary.

    Windows : LGC_API.dll   (+ LGC_API.lib for linking)
    Linux   : libLGC_API.so

## Contents

    LGC_API.dll / libLGC_API.so   native monitoring library (exports the moni* C ABI)
    LGC_API.lib                   (Windows only) import library
    Monitor.h                     header-only C++ facade (class Monitor) over the C ABI
    Monitor_C.h                   flat C ABI header (moni* functions, for C / other FFI)
    pyMonitoring.py               Python ctypes wrapper (class Monitor)
    requirements.txt              Python dependencies (numpy)

## Python (plug and play)

    1. Keep pyMonitoring.py next to the native library (this folder).
    2. pip install -r requirements.txt        # only numpy
    3. use it:
         import sys; sys.path.insert(0, r"<this folder>")
         import pyMonitoring
         m = pyMonitoring.Monitor("project.lgc")
         m.adjust()
         print(m.getSigma0())

No compiler and no pybind are needed - pyMonitoring.py loads the native library
directly via ctypes.

## C++ (header-only facade)

`Monitor.h` is a header-only facade: it is compiled in YOUR translation unit and
only calls the flat C ABI, so it works with any compiler, C++ runtime and C++
standard - it does NOT have to match the toolchain the library was built with.
It includes <Eigen/Dense>; point the compiler at your own Eigen (header-only, not
shipped). Because only C types cross the boundary, your Eigen version is entirely
your own choice.

    Windows (MSVC):
      cl /std:c++17 /EHsc /I "<this folder>" /I "<your Eigen>" app.cpp LGC_API.lib
      # ship LGC_API.dll next to your .exe

    Linux (gcc/clang):
      g++ -std=c++17 -I "<this folder>" -I "<your Eigen>" app.cpp \
          -L "<this folder>" -lLGC_API -Wl,-rpath,'$ORIGIN'
      # keep libLGC_API.so next to your executable

    app.cpp:
      #include "Monitor.h"
      int main() { Monitor m("project.lgc"); m.adjust(); }

## C / other languages (flat C ABI)

Use `Monitor_C.h` (the `extern "C"` `moni*` functions) and load
LGC_API.dll / libLGC_API.so directly from any FFI (ctypes, MATLAB loadlibrary,
Julia ccall, ...). No Eigen needed - the C ABI passes plain `double*` arrays.
