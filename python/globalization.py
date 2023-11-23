import pyLGC
import numpy as np
import scipy.sparse as sp
from analyzer import huberSolver,huberSolution, prepend_to_line, attemptBlunderRemoval
from armijo import armijoSolver
import matplotlib.pyplot as plt

import pyomo.environ as pyo
from pyomo.contrib.incidence_analysis import IncidenceGraphInterface
import pyomo




#evaluator = pyLGC.LGCController("huberExamples/170712_14736_XBPF_22716_PLR3D.lgc")
#evaluator = pyLGC.LGCController("huberExamples/PSR2AD-NTOF_CALA-PSR-AD.lgc")
#evaluator = pyLGC.LGCController("huberExamples/20697_V2_Dirk_ESA_experimental_Blunder.lgc")
#evaluator = pyLGC.LGCController("huberExamples/SUS-1795_ba4_polygo_3cala_cdcg.lgc")
#evaluator = pyLGC.LGCController("huberExamples/minimal.lgc")
evaluator = pyLGC.LGCController("huberExamples/totalStationConvergence.lgc")
#evaluator = pyLGC.LGCController("huberExamples/minimal_diverging.lgc")

par=evaluator.getParameter()
## parDim=par.shape[0]
## obsDim=evaluator.getW(par).shape[0]
## actObsIndices = list(range(obsDim))
## huberObject=huberSolver(evaluator)
## sol1=huberObject.solve(par,2.95)
## sol1.showQQPlot(evaluator)
## # find biggest blunder index
## linPart=sol1.posLinearPart+sol1.negLinearPart
## idx=np.argmax(linPart)
## actObsIndices.remove(idx)
## # deactivate this index
## print(evaluator.getPv(par).shape)
## evaluator.setObservationMask(actObsIndices)
## print(evaluator.getPv(par).shape)
## print(evaluator.getA(par).shape)
## print(evaluator.getB(par).shape)
## print(evaluator.getA2(par).shape)
## print(evaluator.getW(par).shape)
## print(evaluator.getW2(par).shape)
## 
## # solve again
## sol2=huberObject.solve(par,2.95)
## sol2.showQQPlot(evaluator)
## 

print("solve with armijo")
arm=armijoSolver(evaluator)
leastSquareSol = arm.solve(par)
print("solve with huber")
huber=huberSolver(evaluator)
huberSol=huber.solve(par,2.95)
#huberSol.showQQPlot(evaluator)
print("diff Norm=",np.linalg.norm(leastSquareSol-huberSol.primalSolution))
plt.show()




A=evaluator.getA(par)
plt.figure(figsize=(8, 8))
plt.spy(A)
plt.title('Sparsity Pattern')
plt.xlabel('Column Index')
plt.ylabel('Row Index')
plt.show()
# get max matching
maxMatch=pyomo.contrib.incidence_analysis.matching.maximum_matching(A.tocoo())
rowIdx=maxMatch.values()
Areduced=A[list(maxMatch.values()),:]

print(A.todense())
plt.spy(Areduced)
#[rowList,colList]=pyomo.contrib.incidence_analysis.triangularize.block_triangularize(A)
