
# basic usage of pyLGC dll to get and set parameters + evaluate misclosures and derivatives

# place the pyLGC.pyd in this folder
import pyLGC
import numpy as np
import scipy.sparse as sp
from scipy import sparse
from scipy.io import mmread
import matplotlib.pyplot as plt

obj = pyLGC.LGCController("Title-Example.lgc")

# save the currently set parameter
parameter=obj.getParameter()
print("Parameter is currently set to p=",parameter)
originalMisclosure=obj.getW(parameter)
print("misclosure(p)=",originalMisclosure)
A=obj.getA(parameter)
# A matrix is in sparse matrix format, only nonzeros saved
# to print a small matrix, optionally convert it to dense
print("A matrix d misclosure / dp (p)=",obj.getA(parameter).toarray())

# test plausibility of (i,j) entry in the A matrix
epsilon=1e-6
# row
i=4
# col
j=1
parameter[j]+=epsilon
# miscl(p+epsilon e_j)
perturbedMisclosure = obj.getW(parameter)
# compute the derivative via finite difference
Aij_finiteDiff=(perturbedMisclosure[i]-originalMisclosure[i])/epsilon
print("row index i=", i, " column index j= ",j)
print("A[i,j] via finite diff approx= ", Aij_finiteDiff)
print("A[i,j] via LGC internal code= ", A[i,j])



True