import pyLGC
import numpy as np
import scipy.sparse as sp
from huberTools import huberSolver,huberSolution, prepend_to_line, attemptBlunderRemoval
from armijo import armijoSolver
import matplotlib.pyplot as plt
from structuralDecompositionTools import sparsityAnalyzer


# seed for random number generation
np.random.seed(0)

#evaluator = pyLGC.LGCController("huberExamples/170712_14736_XBPF_22716_PLR3D.lgc")
#evaluator = pyLGC.LGCController("huberExamples/PSR2AD-NTOF_CALA-PSR-AD.lgc")
#evaluator = pyLGC.LGCController("huberExamples/20697_V2_Dirk_ESA_experimental_Blunder.lgc")
#evaluator = pyLGC.LGCController("huberExamples/20190814_10h06_Polar_Module_ATLAS_V2.lgc")
#evaluator = pyLGC.LGCController("huberExamples/SUS-1795_ba4_polygo_3cala_cdcg.lgc")
#evaluator = pyLGC.LGCController("huberExamples/minimal.lgc")
#evaluator = pyLGC.LGCController("huberExamples/totalStationConvergence.lgc")
#evaluator = pyLGC.LGCController("huberExamples/minimal_diverging.lgc")
evaluator = pyLGC.LGCController("huberExamples/ConvergenceProblemUVEC.lgc2")

# get the first design matrix to perform a sparsity structure analysis
par=evaluator.getParameter()
dimPar=par.shape[0]
par=par#+1*np.random.rand(dimPar)
A=evaluator.getA(par)
analysisTool= sparsityAnalyzer(A)

# prepare a solver object
armijoWorker=armijoSolver(evaluator)
solution=armijoWorker.solve(par)
huberWorker=huberSolver(evaluator)
solution= huberWorker.solve(par,2.95,1000,0.1)
print(solution.primalSolution)

currentParIdx=list()
currentEqIdx=list()
# loop over the decomposed blocks
blockNumber=0
for eqBlock,parBlock in analysisTool.decompositionList:
    blockNumber+=1
    print("Attempt solving block number ", blockNumber," of size (eqs,pars)=",len(eqBlock),",",len(parBlock))
    currentEqIdx+=eqBlock
    currentParIdx+=parBlock
    # set masks
    evaluator.setObservationMask(currentEqIdx)
    test=evaluator.getObservationMask()
    evaluator.setParameterMask(currentParIdx)
    testMask=evaluator.getObservationMask()
    currentPar=evaluator.getParameter()
    # check rank
    aux=evaluator.getA(currentPar).toarray()
    Arank=np.linalg.matrix_rank(aux)
    #print(aux)
    print("A matrix has rank",np.linalg.matrix_rank(aux))
    if (Arank>=len(currentParIdx)):
        # try to solve
        sol=armijoWorker.solve(currentPar)
        huberSol=huberWorker.solve(currentPar,2.95)
        huberSol.showQQPlot(evaluator)
plt.show()

True