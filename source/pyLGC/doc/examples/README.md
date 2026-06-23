# pyLGC Examples

The Python scripts in this folder serve as clean, readable code snippets that are dynamically injected into the `pyLGC_manual.pdf` LaTeX documentation. 

Because they act as documentation snippets, they are intentionally kept simple and **cannot be run directly from this folder** out-of-the-box. If you try to run them here, Python will fail to find both the `pyLGC` module and the `Title-Example.lgc2` data file.

### How to run these examples

If you want to execute these scripts interactively, the easiest method is to copy them into your build directory:

1. Ensure you have built the `pyLGC_C` DLL (see the main pyLGC README located at `source/pyLGC/README.md`).
2. Copy the example `.py` file you want to run (e.g., `01_basic_workflow.py`) into your compiled tests directory: `build/pyLGC/tests/`.
3. Open a terminal in `build/pyLGC/tests/` and run the script using `uv`:
   
   ```powershell
   uv run python 01_basic_workflow.py
   ```

By running them from the compiled `tests` folder, the script will automatically have access to the `pyLGC.py` wrapper, the compiled `pyLGC_C.dll`, and the required `Title-Example.lgc2` test data file.
