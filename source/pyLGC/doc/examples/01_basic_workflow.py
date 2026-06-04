"""Basic workflow: load, evaluate, inspect, solve."""

import pyLGC

# 1. Load
ev = pyLGC.Evaluator("Title-Example.lgc2")
idx = ev.getProblemDimensions()
print(f"Unknowns: {idx.UIndex}, Equations: {idx.EIndex}")

# 2. Evaluate at provisional parameters
ev.evaluateAtParameters()

# 3. Inspect the linearised system
A = ev.getFirstDesignMatrix()       # (rows, cols, vals, nrows, ncols)
w = ev.getMisclosure()    # numpy array of length EIndex

# 4. Solve
ok, solution = ev.solve()
if ok:
    # 5. Evaluate at the solution
    ev.setParameters(solution)
    ev.evaluateAtParameters()
    w_at_solution = ev.getMisclosure()
