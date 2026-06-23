"""Converting COO tuples to SciPy sparse matrices."""

from scipy.sparse import coo_matrix
import pyLGC

ev = pyLGC.Evaluator("Title-Example.lgc2")
ev.evaluate()

# Convert COO tuple to scipy sparse matrix
rows, cols, vals, nr, nc = ev.getFirstDesignMatrix()
A = coo_matrix((vals, (rows, cols)), shape=(nr, nc)).tocsr()

rows, cols, vals, nr, nc = ev.getWeightMatrix()
P = coo_matrix((vals, (rows, cols)), shape=(nr, nc)).tocsr()

w = ev.getMisclosure()
