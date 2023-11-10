## simple implementation of a huber estimator using pyLGC and proxsuite

import seaborn as sns
import statsmodels.api as sm
import proxsuite
from quadprog import solve_qp
import clarabel
import pyLGC
import numpy as np
import scipy.sparse as sp
from scipy import sparse
from scipy.io import mmread
import matplotlib.pyplot as plt
from analyzer import huberSolver,huberSolution

#obj = pyLGC.LGCController("robust.lgc")
#obj = pyLGC.LGCController("inclTest.lgc")
#obj = pyLGC.LGCController("SUS-1790_23826_reseau_VV.lgc")
#obj = pyLGC.LGCController("PSR2AD-NTOF_CALA-PSR-AD_noPLR3D.lgc")
#obj = pyLGC.LGCController("12492_lgcv2_brut.lgc")
#obj = pyLGC.LGCController("AR_LIBRE.lgc")
#obj = pyLGC.LGCController("simple.lgc")
#obj = pyLGC.LGCController("tstnHuber_NoBlunder.lgc")
#obj = pyLGC.LGCController("tstnHuber_NoBlunder.sim")
#obj = pyLGC.LGCController("LHC_LGC2_D2M-Q2S_OLOC.sim")
#obj = pyLGC.LGCController("LHC_LGC2_D2M-Q2S_OLOC.inp")
#obj = pyLGC.LGCController("15925-v5.lgc")
#obj = pyLGC.LGCController("LHC_2013-Compense_v2-LGC_1.12.02.lgc")
#obj = pyLGC.LGCController("LHC_2013-Compense_v2-LGC_1.12.02.sim")
#obj = pyLGC.LGCController("huberExamples/PSR2AD-NTOF_CALA-PSR-AD.lgc")
#obj = pyLGC.LGCController("huberExamples/28360.lgc")
#obj = pyLGC.LGCController("PSR2AD-NTOF_CALA-PSR-AD.sim")
obj = pyLGC.LGCController("huberExamples/12731-LSS5_Gyro-Acc_GKA_TestVersion_CALA.lgc")
#obj = pyLGC.LGCController("huberExamples/simpleMinimal.lgc")
#obj = pyLGC.LGCController("huberExamples/SUS-1910_ScanLGC_frames_OBSXYZ_OK_BF.lgc")

myTool= huberSolver(obj)
initialValue = obj.getParameter()
#for huberGamma in [10,5,2,1,0.0001]:
#for huberGamma in [1.5]:
for huberGamma in [2.95,100000]:
    print("Attempt Huber problem with gamma =",huberGamma)
    solution= myTool.solve(initialValue, huberGamma,maxIter=10)
    solution.showQQPlot(obj)
    diffNorm=np.linalg.norm(solution.primalSolution-initialValue)
    linPart=solution.negLinearPart+solution.posLinearPart
    #print("linear part=",np.linalg.norm(linPart))
    #print("solution=",solution.primalSolution)
    # print("V,W,R,S")
    # print(solution.residual[646],solution.quadraticPart[646],solution.posLinearPart[646],solution.negLinearPart[646])
    numberLinearContributions=np.sum(np.abs(linPart)>1e-4)
    
    print("number of linear contributions = ",numberLinearContributions)
    print("difference to previous huber solution=",diffNorm)
    initialValue=solution.primalSolution


    
#rs = x[-2*nObs:]
#print(rs)
#print("number of potential outliers=", np.count_nonzero(np.abs(rs)>2))
#indices=np.where(np.abs(rs)>0.1)
#print(indices,rs[indices])
#print("solution=",x[:nPar])

#solution.showQQPlot()

#P=obj.getPv(solution.primalSolution)
#weight=np.sqrt(P)
### weighted residuals
#residuals=weight*solution.residual
#absResiduals=np.abs(residuals)
#max_value = absResiduals.max()
#max_index = np.argmax(absResiduals)
#n = 5
#indices = np.argpartition(absResiduals, -n)[-n:]
#sortedIndices = indices[np.argsort(-absResiduals[indices])]
#
#linParts = solution.negLinearPart+solution.posLinearPart
#linIndices = np.argpartition(linParts, -n)[-n:]
#sortedLinIndices=linIndices[np.argsort(-linParts[linIndices])]
#print("biggest linear parts=",linParts[sortedLinIndices])
#print("indices of biggest linear parts=",sortedLinIndices)
#
#
#print(n," biggest abs residuals",absResiduals[sortedIndices])
#print(" linearParts",solution.negLinearPart[sortedIndices]+solution.posLinearPart[sortedIndices])
#
#print("at obs-indices ",sortedIndices)
#
#
#
#print("Maximum value:", max_value, " at index ",max_index)
#sm.qqplot(residuals, line ='45') # 
#
## Create the histogram with custom bins
##plt.xscale('symlog')
#
## Show the plot
#plt.show()

## 
## obsIndex=2
## v=solution.residual
## w=solution.quadraticPart
## r=solution.posLinearPart
## s=solution.negLinearPart
## P=obj.getPv(solution.primalSolution)
## weight=np.sqrt(P)
## #print("Sv * v -w -r +s \n")
## #print(weight,"*",v,"-",w,"-",r,"+",s,"\n")
## #print(w-r+s)
## #print(weight@v-w-r+s)
## #print(solution.fullSol)
## #


# to prevent closing the windows immeadiatelly
plt.show()

True