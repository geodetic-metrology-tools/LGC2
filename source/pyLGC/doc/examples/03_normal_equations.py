"""Forming and solving the normal equations."""

from scipy.sparse import coo_matrix
from scipy.sparse.linalg import spsolve
import pyLGC

ev = pyLGC.Evaluator("Title-Example.lgc2")
ev.evaluateAtParameters()

# Build sparse matrices
def to_csr(tup):
    rows, cols, vals, nr, nc = tup
    return coo_matrix((vals, (rows, cols)), shape=(nr, nc)).tocsr()

A = to_csr(ev.getFirstDesignMatrix())
P = to_csr(ev.getWeightMatrix())
w = ev.getMisclosure()

# Normal equations: (A^T P A) dx = -A^T P W
N = A.T @ P @ A
n = A.T @ P @ w
dx = spsolve(N, -n)
