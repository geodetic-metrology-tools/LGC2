# testing matrix analyzer class
import pyLGC
import numpy as np
import scipy as sp
import random
from  structuralDecompositionTools import create_sparse_matrix_with_blocks, apply_random_permutation, createSparsityTransformationAnimation, plot_sparsity_pattern, sparsityAnalyzer
import matplotlib.pyplot as plt

# allow code continuation without closing plot figures
plt.ion()

#evaluator = pyLGC.LGCController("huberExamples/totalStationConvergence.lgc")
evaluator = pyLGC.LGCController("huberExamples/SUS-1895_26465_TT2-radial_calage-FTN.lgc")
#evaluator = pyLGC.LGCController("huberExamples/test_With_Meas_GKA.lgc")
par=evaluator.getParameter()
A=evaluator.getA(par)
plot_sparsity_pattern(A)
plt.show()
#B=apply_random_permutation(A)
#plot_sparsity_pattern(B,"randomized test matrix")
#plt.show()
myAnalyzer = sparsityAnalyzer(A)
test=myAnalyzer.decompositionList

eqnSequence=list()
parSequence=list()
for eqSet,parList in test:
    for eqIdx in eqSet:
        eqnSequence.append(eqIdx)
    for parIdx in parList:
        parSequence.append(parIdx)

auxMat=A[eqnSequence,:]
auxMat=auxMat[:,parSequence]
#btf=myAnalyzer.getBTF()
#flattened_eq = [item for sublist in btf[0] for item in sublist]
#flattened_par = [item for sublist in btf[1] for item in sublist]
#auxMat = A[flattened_eq][flattened_par]
plot_sparsity_pattern(auxMat)
plt.show()
