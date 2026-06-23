# pyLGC Documentation

This folder contains the source code for the `pyLGC` API manual.

## Compiling the PDF

To compile the manual into a PDF, you will need a LaTeX distribution installed on your system (e.g., [MiKTeX](https://miktex.org/download) or [TeX Live](https://tug.org/texlive/) for Windows).

Once installed, open your terminal in this directory and run the following command:

```powershell
pdflatex pyLGC_manual.tex
```

**Important:** You typically need to run the command **twice** in a row. The first pass generates the auxiliary files necessary to build the Table of Contents and resolve references. The second pass will inject them into the final PDF.

## Dynamic Code Examples

The code examples in the manual are NOT hardcoded into `pyLGC_manual.tex`. 

Instead, the LaTeX file dynamically pulls the Python code directly from the `examples/` folder using the `\lstinputlisting` command. 

If you update any of the `.py` scripts in the `examples/` directory, simply recompile the PDF with `pdflatex` and the manual will automatically reflect the newest code.
