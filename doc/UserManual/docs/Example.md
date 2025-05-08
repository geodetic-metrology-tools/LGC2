# Advanced Markdown Example

This document demonstrates a few advanced documentation features using MkDocs and the Material theme.

---
## Math Test

$$
x = {-b \pm \sqrt{b^2 - 4ac} \over 2a}
$$


## 🧮 A Complex LaTeX Equation

Here's a matrix equation written in LaTeX. To render this properly, enable the `pymdownx.arithmatex` extension in `mkdocs.yml`.


$$
\mathbf{X} =
\begin{bmatrix}
x_1 & x_2 \\
x_3 & x_4
\end{bmatrix}
\cdot
\begin{bmatrix}
y_1 \\
y_2
\end{bmatrix}
=
\begin{bmatrix}
x_1 y_1 + x_2 y_2 \\
x_3 y_1 + x_4 y_2
\end{bmatrix}
$$

---

## 🖼️ Embedded Image

Here’s the project logo:

![LGC Logo](assets/logo.png)

Make sure the `logo.png` file exists in your `assets/` folder.

---

## 🔗 External Link

Learn more about Markdown on its [Wikipedia page](https://en.wikipedia.org/wiki/Markdown).

---

## 📚 Citation (Manual Style)

You can manually cite papers like this:

> Gutekunst, J., Klumb, F., Kautzmann, G., Durand, H. M., & Sosin, M. (2024).  
> *Continuous position estimation for the full remote alignment system of the High Luminosity LHC upgrade*.  
> **JACoW IPAC**, 2024, THPG36. [doi:10.18429/JACoW-IPAC2024-THPG36](https://doi.org/10.18429/JACoW-IPAC2024-THPG36)  
> [CDS record](https://cds.cern.ch/record/2912707)

---

## 🧩 Tip: Enable Math Rendering

To properly render math in MkDocs Material, add this to `mkdocs.yml`:

```yaml
markdown_extensions:
  - pymdownx.arithmatex
extra_javascript:
  - https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js
```
