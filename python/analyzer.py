import clarabel
import pyLGC
import mplcursors
import copy
import numpy as np
import scipy.sparse as sp
from dataclasses import dataclass
import seaborn as sns
import statsmodels.api as sm
import matplotlib.pyplot as plt

def cast2HuberQP(P,A,A2,B,W,W2,huberGamma):
    ## taking the design matrices and the weight  matrix.
    ## huber gamma defines from which multiple of the standard deviation an residual is penalized linearly
    ## setting up the Huber QP
    ## returning Hessian and linear equality constraints+inequality constraints

    # variables are
    # dx,V,W,R,S
    nPar=A.shape[1]
    nEq=A.shape[0]
    nCnstr=A2.shape[0]
    nObs=P.shape[0]
    nHuberPar = nPar+4*nObs
    nHuberEq= nEq+nCnstr+nObs
    nHuberIneq=2*nObs
    ZeroObsxObs =sp.csr_matrix((nObs,nObs))
    Iobs=sp.eye(nObs)
    diagonalElements=P
    roots = np.sqrt(diagonalElements)
    Sv=sp.diags(roots)
    A2fill=sp.csr_matrix((nCnstr,4*nObs))
    # equality constraint matrix 
    Ceq=sp.bmat([[A,B,ZeroObsxObs,ZeroObsxObs,ZeroObsxObs],
                 [A2,None,None,None,None],
                 [sp.csr_matrix((nObs,nPar)),Sv,-Iobs,-Iobs,Iobs]])
    ##Ceq=sp.bmat([[A,B,ZeroObsxObs,ZeroObsxObs,ZeroObsxObs],
    ##               [A2,sp.csr_matrix((nCnstr,4*nObs))],
    ##               [sp.csr_matrix((nObs,nPar)),Sv,-Iobs,-Iobs,Iobs]])
    # inequality constraint matrix 
    Cineq=sp.bmat([[sp.csr_matrix((nObs,nPar+2*nObs)),Iobs,sp.csr_matrix((nObs,nObs))],
                   [sp.csr_matrix((nObs,nPar+2*nObs)),sp.csr_matrix((nObs,nObs)),Iobs]])
    # equality bound
    bEq=np.concatenate((-W,-W2,np.zeros((nObs))))
    # inequality bounds
    BIG =1e+6
    lbIneq=np.zeros(2*nObs)
    ubIneq=BIG*np.ones(2*nObs)
    # hessian 
    hessDiag=np.concatenate((np.zeros(nPar+nObs),np.ones(nObs),np.zeros(2*nObs)))
    hessian=sp.diags(hessDiag)
    # gradient
    gradient=huberGamma*np.concatenate((np.zeros(nPar+2*nObs),np.ones(2*nObs)))
    return [hessian,gradient,Ceq,Cineq,bEq,lbIneq,ubIneq]

def solveQPwithClarabel(hessian, gradient, Ceq, Cineq, bEq, lbIneq, ubIneq):
    ## first tests: this seems to be fast
    nParQP = gradient.shape[0]
    nEqQP = Ceq.shape[0]
    nIneqQP = Cineq.shape[0]
    #nHuberPar = nPar+4*nObs
    #nHuberEq= nEq+nCnstr+nObs
    #nHuberIneq=2*nObs
    AClarabel = sp.bmat([[Ceq], [Cineq], [-Cineq]])
    bClarabel = np.concatenate((bEq, ubIneq, -lbIneq))
    cones = [clarabel.ZeroConeT(nEqQP), clarabel.NonnegativeConeT(2*nIneqQP)]

    settings = clarabel.DefaultSettings()
    #settings.verbose = True
    settings.verbose = False
    sparseHessian=hessian.tocsc()
    solver = clarabel.DefaultSolver(
        sparseHessian, gradient, AClarabel.tocsc(), bClarabel, cones, settings)
    solution = solver.solve()
    # test solution equality constraint
    # print("solution quality",np.linalg.norm(Ceq@solution.x-bEq))
    return np.array(solution.x)

@dataclass
class huberSolution:
    # primal solution of the estimation problem
    primalSolution: np.array
    # v=residual
    residual:np.array
    # w=quadratic weighted part
    quadraticPart: np.array
    # r,s the linear parts: r the positive part, s negative, one of them should be enforced zero by the huber solver
    # so together : sqrt(Pv)v-w=r-s
    posLinearPart:np.array
    negLinearPart:np.array
    fullSol:np.array
    # including the weight matrix at the solution. it contains useful information for the analysis of the solution
    weightDiagonal:np.array
    # the huber gamma of the problem
    huberGamma: float

    def showQQPlot(self, evaluator):
        # generate a theoretical quantile vs real quantile plot
        fig,ax=plt.subplots()
        yData= self.weightDiagonal*self.residual
        sm.qqplot(yData, line ='45',ax=ax) # 
        plt.title("Q-Q Plot of Residuals. Huber Gamma="+str(self.huberGamma))
        linPart=self.negLinearPart+self.posLinearPart
        numberLinearContributions=np.sum(np.abs(linPart)>1e-4)
        plt.xlabel("Theoretical Quantiles."+str(numberLinearContributions)+" Residuals above Huber threshold.")
        plt.ylabel("Sample Quantiles")

        # Draw a horizontal line at the specified height
        plt.axhline(y=self.huberGamma, color='green', linestyle='--',label="Huber Threshold "+str(self.huberGamma))
        plt.axhline(y=-self.huberGamma, color='green', linestyle='--')

        # Extract the line data (theoretical and actual quantiles)
        line = ax.get_lines()[0]

        plt.legend()

        # # Get the data points from the plot
        # qq_line = ax.get_lines()[0]
        # x = qq_line.get_xdata()
        # y = qq_line.get_ydata()

        # # Color points above the threshold in red
        # above_threshold = self.residual > self.huberGamma
        # ax.scatter(x[above_threshold], y[above_threshold], color='red',s=100)
        # #ax.scatter(x[10], y[10], color='red',s=100)

        # Show the plot
        plt.show(block=False)

                # Add interactive cursor
        cursor = mplcursors.cursor(ax, hover=True)
        @cursor.connect("add")
        def on_add(sel):
             # Find the index of the point closest to the cursor
            distance = np.sqrt((yData - sel.target[1])**2)
            index = np.argmin(distance)
            if (distance[index]<0.001):
                sel.annotation.set(text=f"ObservationIndex:{index} Line Number:{evaluator.getLineNumber(index)}")
            else:
                sel.annotation.set(text=f"No data point.")
            #sel.annotation.set(text=f"ObservationIndex: {index}")




class huberSolver:
    def __init__(self, evaluator):
        self._evaluator = evaluator

    def solve(self,initialValue,gamma,maxIter=50):
        #solving the huber estimation problem with threshold huberGamma
        evaluator = self._evaluator
        par=copy.deepcopy(initialValue)
        it=0
        dx=1
        while (it<maxIter)& (np.linalg.norm(dx)>1e-4):
            it+=1
            A=evaluator.getFirstDesignMatrix(par)
            A2=evaluator.getFirstConstraintDesignMatrix(par)
            B=evaluator.getSecondDesignMatrix(par)
            Pv=evaluator.getPv(par)
            #print(Pv)
            W=evaluator.getMisclosure(par)
            W2=evaluator.getConstraintMisclosure(par) 
            [hessian, gradient, Ceq, Cineq, bEq, lbIneq,
                ubIneq] = cast2HuberQP(Pv, A, A2, B, W, W2,gamma)
            xClara = solveQPwithClarabel(hessian, gradient, Ceq, Cineq, bEq, lbIneq, ubIneq)
            dx = xClara[:A.shape[1]]

            par+=dx
            ##print(rs)
            print("|dx|=",np.linalg.norm(dx))
            ##print("x=",par)
            ##print("x+dx=",par) 
        
        # extract the huber specific parts
        nPar=A.shape[1]
        nObs=A.shape[0]
        nCnstr=A2.shape[0]
        # residual
        v=xClara[nPar:nPar+nObs]
        # quadratic part
        w=xClara[nPar+nObs:nPar+2*nObs]
        # linear parts
        r=xClara[nPar+2*nObs:nPar+3*nObs]
        s=xClara[nPar+3*nObs:nPar+4*nObs]
        fullSol=xClara

        
        return huberSolution(par,v,w,r,s,fullSol, np.sqrt(Pv),gamma)
