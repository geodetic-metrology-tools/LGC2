## simple implementation of a huber estimator using pyLGC and proxsuite

import seaborn as sns
import statsmodels.api as sm
import clarabel
import pyLGC
import numpy as np
import scipy.sparse as sp
from scipy import sparse
from scipy.io import mmread
import matplotlib.pyplot as plt
from huberTools import huberSolver,huberSolution, prepend_to_line, attemptBlunderRemoval

# example from Dirks Presentation, see SUS-2235
obj = pyLGC.LGCController("huberExamples/20697_V2_Dirk_ESA_experimental.lgc")

myTool= huberSolver(obj)
initialValue = obj.getParameter()
for huberGamma in [2.95,10000000]:
    print("Attempt Huber problem with gamma =",huberGamma)
    solution= myTool.solve(initialValue, huberGamma,maxIter=100)
    solution.showQQPlot(obj)
    diffNorm=np.linalg.norm(solution.primalSolution-initialValue)
    linPart=solution.negLinearPart+solution.posLinearPart
    #print("linear part=",np.linalg.norm(linPart))
    #print("solution=",solution.primalSolution)
    # print("V,W,R,S")
    # print(solution.residual[646],solution.quadraticPart[646],solution.posLinearPart[646],solution.negLinearPart[646])
    numberLinearContributions=np.sum(np.abs(linPart)>1e-6)
    
    print("number of linear contributions = ",numberLinearContributions)
    print("difference to previous huber solution=",diffNorm)
    initialValue=solution.primalSolution

filename="huberExamples/20697_V2_Dirk_ESA_experimental.lgc"
#filename = "huberExamples/20190814_10h06_Polar_Module_ATLAS_Blunder.lgc"
attemptBlunderRemoval(filename)

# to prevent closing the windows immediatelly
plt.show()

True