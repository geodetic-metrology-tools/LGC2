"""Comparing analytic and numerical Jacobians."""

import numpy as np
from scipy.sparse import coo_matrix
import pyLGC

ev = pyLGC.Evaluator("Title-Example.lgc2")
ok, sol = ev.tryLGCSolve()
ev.setParameters(sol)
ev.evaluate()

# Analytic Jacobian
r, c, v, nr, nc = ev.getAMatrix()
A_analytic = coo_matrix((v, (r, c)), shape=(nr, nc)).toarray()

# Numerical Jacobian (C++ internal finite differences)
A_fd = ev.getFiniteDifferenceA(1e-7)

# Compare
diff = np.abs(A_analytic - A_fd)
print(f"Max difference: {diff.max():.2e}")
print(f"Mean difference: {diff.mean():.2e}")
