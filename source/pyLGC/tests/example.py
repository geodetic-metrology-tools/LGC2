"""Toy example: load Title-Example.lgc2 and inspect the LS problem."""

import numpy as np
from scipy.sparse import coo_matrix
import pyLGC

lgc_file = "Title-Example.lgc2"

# Load the input file and build the evaluator
ev = pyLGC.Evaluator(lgc_file)

# Problem dimensions
idx = ev.getProblemDimensions()
print(f"Unknowns: {idx.UIndex}, Equations: {idx.EIndex}, Observations: {idx.OIndex}")

# Inspect points and frames
for name in ["H4.XBPF.22716.E", "H4.XBPF.22716.S"]:
    pt = ev.getPoint(name)
    print(f"Point '{pt.getName()}': firstUidx={pt.getFirstUidx()}, active={pt.getRelativeUnknIndices()}")

for fname in ["ROOT", "testFrame"]:
    frame = ev.getFrame(fname)
    active = frame.getRelativeUnknIndices()
    if len(active) > 0:
        print(f"Frame '{frame.getName()}': firstUidx={frame.getFirstUidx()}, active={active}")
    else:
        print(f"Frame '{frame.getName()}': fully fixed")

# Evaluate at provisional parameters
ev.evaluateAtParameters()
params = ev.getEstimatedParameters()
print(f"Parameter vector ({len(params)} elements): {params[:6]}...")

# Design matrix (COO tuple)
A_tup = ev.getFirstDesignMatrix()
print(f"A matrix: ({A_tup[3]}, {A_tup[4]}), nnz={len(A_tup[2])}")

# Misclosure
w = ev.getMisclosure()
print(f"Misclosure vector ({len(w)} elements): {w[:6]}...")

# Solve
ok, solution = ev.solve()
if ok:
    print(f"Solve succeeded")
else:
    print("Solve failed")
