"""Simple Gauss-Newton solver, validated against the built-in solver."""

import numpy as np
from numpy.linalg import norm
from scipy.sparse import coo_matrix
from scipy.sparse.linalg import spsolve
import pyLGC

def to_csr(tup):
    rows, cols, vals, nr, nc = tup
    return coo_matrix((vals, (rows, cols)), shape=(nr, nc)).tocsr()

ev = pyLGC.Evaluator("Title-Example.lgc2")

# 1. Reference solution from the built-in solver
x0 = ev.getEstimatedParameters().copy()
ok, x_ref = ev.solve()

# 2. Reset to provisional values and run own Gauss-Newton
x = x0.copy()
for k in range(50):
    ev.setParameters(x)
    ev.evaluateAtParameters()

    A = to_csr(ev.getFirstDesignMatrix())
    P = to_csr(ev.getWeightMatrix())
    w = ev.getMisclosure()

    # Solve normal equations: (A^T P A) dx = -A^T P w
    N = A.T @ P @ A
    dx = spsolve(N, -A.T @ P @ w)
    x = x + dx

    print(f"iter {k:2d}  |dx| = {norm(dx):.2e}  |w| = {norm(w):.2e}")
    if norm(dx, np.inf) < 1e-8:
        print("Converged.")
        break

# 3. Compare the two solutions
print(f"||x_gn - x_ref|| = {norm(x - x_ref):.2e}")
