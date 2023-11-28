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
evaluator = pyLGC.LGCController("huberExamples/20190814_10h06_Polar_Module_ATLAS_V2.lgc")
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

# find all eqn related to P1 +P2
#P1=btf[1][0]
#P2=btf[1][1]
#union=P1.union(P2)
#eqns=myAnalyzer.getAllAssociatedEqn(union)
fullSet=set(range(myAnalyzer.get_shape()[1]))
eqns=myAnalyzer.getAllAssociatedEqn(fullSet)
print("P1=",P1)
#print("P2=",P2)
print("eqns=",eqns),

print(flattened_eq)
print(flattened_par)

createSparsityTransformationAnimation(A,flattened_eq,flattened_par)

print(B.todense())
A_recovered = B.todense()
print(A_recovered.shape)
A_recovered=A_recovered[flattened_eq,:]
A_recovered=A_recovered[:,flattened_par]
#A_recovered=A_recovered[:,flattened_eq]
print(A_recovered.shape)
print(A_recovered)
#A_recovered = (A_recovered[:,flattened_par]).tocoo()
#A_recovered = (B.tocsr()[flattened_eq,:])#.tocoo()
#A_recovered = (A_recovered[:,flattened_par]).tocoo()
#plt.figure()
plot_sparsity_pattern(sp.sparse.coo_matrix(A_recovered),"recovered matrix")
plt.show()
True
