"""Toy example: load Title-Example.lgc2 and inspect the LS problem."""

import numpy as np
from scipy.sparse import coo_matrix
import pyLGC

lgc_file = "Title-Example.lgc2"

# Load the input file and build the evaluator
ev = pyLGC.Evaluator(lgc_file)

# Problem dimensions
idx = ev.getIndices()
print(f"Unknowns: {idx.UIndex}, Equations: {idx.EIndex}, Observations: {idx.OIndex}")

# Inspect points and frames
for name in ["H4.XBPF.22716.E", "H4.XBPF.22716.S"]:
    pt = ev.getPoint(name)
    print(f"Point '{pt.getName()}': firstUidx={pt.getFirstUidx()}, active={pt.getRelativeUnknIndices()}")

for fname in ["ROOT", "testFrame"]:
    frame = ev.getFrame(fname)
    active = frame.getRelativeUnknIndices()
    if active:
        print(f"Frame '{frame.getName()}': firstUidx={frame.getFirstUidx()}, active={active}")
    else:
        print(f"Frame '{frame.getName()}': fully fixed")

# Evaluate at provisional parameters
ev.evaluate()
params = ev.getEstParams()
print(f"Parameter vector ({len(params)} elements): {params[:6]}...")

# Design matrix (COO tuple)
A_tup = ev.getAMatrix()
print(f"A matrix: ({A_tup[3]}, {A_tup[4]}), nnz={len(A_tup[2])}")

# Misclosure
w = ev.getMisclosure()
print(f"Misclosure vector ({len(w)} elements): {w[:6]}...")

# Solve
ok, solution = ev.tryLGCSolve()
if ok:
    print(f"Solve succeeded")

    # Test derivatives at the solution using a fresh evaluator
    ev2 = pyLGC.Evaluator(lgc_file)
    ev2.setParameters(solution)
    ev2.evaluate()

    A_tup = ev2.getAMatrix()
    w0 = ev2.getMisclosure()
    params = ev2.getEstParams()
    eps = 1e-7
    n_eq, n_unk = A_tup[3], A_tup[4]

    # Collect Python finite difference columns (all C++ work before numpy import)
    fd_columns_py = []
    for j in range(n_unk):
        p = list(params)
        p[j] += eps
        ev2.setParameters(p)
        ev2.evaluate()
        fd_columns_py.append(list(ev2.getMisclosure()))

    # C++ finite difference A matrix (separate evaluator, result as list before numpy)
    ev3 = pyLGC.Evaluator(lgc_file)
    ev3.setParameters(solution)
    ev3.evaluate()
    A_fd_cpp_raw = ev3.getFiniteDifferenceA(eps)
    # convert to nested list while still in C++-safe territory
    A_fd_cpp_list = [list(row) for row in A_fd_cpp_raw]

    rows, cols, vals, nrows, ncols = A_tup
    A_analytic = coo_matrix((vals, (rows, cols)), shape=(nrows, ncols)).toarray()
    w0 = np.array(w0)
    A_fd_cpp = np.array(A_fd_cpp_list)

    A_fd_py = np.zeros((n_eq, n_unk))
    for j in range(n_unk):
        A_fd_py[:, j] = (np.array(fd_columns_py[j]) - w0) / eps

    np.set_printoptions(precision=6, linewidth=200, suppress=True)

    print("\nAnalytic A matrix:")
    print(A_analytic)
    print("\nPython finite difference A matrix:")
    print(A_fd_py)
    print("\nC++ finite difference A matrix:")
    print(A_fd_cpp)

    diff_py = A_analytic - A_fd_py
    diff_cpp = A_analytic - A_fd_cpp
    diff_py_cpp = A_fd_py - A_fd_cpp

    print("\nDifference (analytic - Python fd):")
    print(diff_py)
    print(f"  max = {np.abs(diff_py).max():.2e}")

    print("\nDifference (analytic - C++ fd):")
    print(diff_cpp)
    print(f"  max = {np.abs(diff_cpp).max():.2e}")

    print("\nDifference (Python fd - C++ fd):")
    print(diff_py_cpp)
    print(f"  max = {np.abs(diff_py_cpp).max():.2e}")
else:
    print("Solve failed")
