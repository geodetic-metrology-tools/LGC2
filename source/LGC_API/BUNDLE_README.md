# LGC2 Monitor library - bundle

Self-contained bundle for BOTH the Python monitoring API and direct C/C++ use of
the monitoring library. A single native library serves all of them (it carries
the C++ `Monitor` class and the flat `moni*` C ABI):

    Windows : LGC_API.dll   (+ LGC_API.lib for C++ linking)
    Linux   : libLGC_API.so

## Contents

    LGC_API.dll / libLGC_API.so   native monitoring library (also exports the C ABI)
    LGC_API.lib                   (Windows only) import library for C++ linking
    pyMonitoring.py               Python ctypes wrapper (class Monitor)
    requirements.txt              Python dependencies (numpy)
    Monitor.h                  C++ header (class Monitor)
    pyMonitoring_C.h              flat C ABI header (moni* functions, for C / other FFI)

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

## C++ (link against the library)

`Monitor.h` includes <Eigen/Dense>, so Eigen (header-only) is required at
compile time. Eigen is NOT shipped in this bundle - point the compiler at your
own Eigen.

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

Use `pyMonitoring_C.h` (the `extern "C"` `moni*` functions) and load
LGC_API.dll / libLGC_API.so directly from any FFI (ctypes, MATLAB loadlibrary,
Julia ccall, ...). No Eigen needed - the C ABI passes plain `double*` arrays.
