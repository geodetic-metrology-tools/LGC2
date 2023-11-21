import clarabel
import pyLGC
#import mplcursors
import copy
import numpy as np
import scipy.sparse as sp
import scipy
from dataclasses import dataclass
import matplotlib.pyplot as plt
import shutil
import os
from analyzer import solveQPwithClarabel

class armijoSolver:
    def __init__(self, evaluator):
        # initialize with an evaluator instance
        self._evaluator=evaluator
    def solve(self,initialValue,maxIter=50):
        # Gauss Newton with armijo linesearch
        evaluator = self._evaluator
        par=copy.deepcopy(initialValue)
        it=0
        dx=1
        #direction = self.getGNDirection(par)
        while (it<maxIter)& (np.linalg.norm(dx)>1e-6):
            it+=1
            currentObjective = self.evalObj(par)
            dx=self.getGNDirection(par)
            stepsize = self.ArmijoLinesearch(par, currentObjective, dx)
            par+=stepsize*dx
            print("|dx|=",np.linalg.norm(dx),"alpha=",stepsize)

        return par
    
    def ArmijoLinesearch(self,x0,sigma0,direction):
        evaluator=self._evaluator
        # tau: reduction in case Armijo–Goldstein condition is not satisfied
        tau = 0.5
        # c: Armijo–Goldstein condition: real descent vs descent predicted by gradient
        c = 0.5 
        expectedDescent = self.getGradient(x0).dot(direction)
        alpha=1
        trialPar = x0+alpha*direction
        evaluator.setParameter(trialPar)
        trialSigma=self.evalObj(trialPar)
        trueDescent = trialSigma-sigma0
        alphaBigEnough =True
        insufficientDescent = c * alpha* expectedDescent < trueDescent 
        #sufficientDescent=True
       	## testing armijo goldstein descent condition (real descent has to be at least stepsize * c * full step expected descent )
        #while (c * alpha* expectedDescent < trueDescent & alpha>0.01):
        while (insufficientDescent & alphaBigEnough):
            # reduce stepsize
            alpha *= tau
            trialPar = x0 + alpha * direction
            trialSigma =self.evalObj(trialPar)
            trueDescent = trialSigma - sigma0
            insufficientDescent = c * alpha* expectedDescent < trueDescent
            alphaBigEnough= alpha>0.01
        return alpha

    
    def evalObj(self,par):
        evaluator=self._evaluator
        residual=evaluator.getW(par)
        Pv=sp.diags(evaluator.getPv(par))
        obj=residual.transpose()@Pv@residual
        return obj 
    
    def getGradient(self,par):
        evaluator=self._evaluator
        A=evaluator.getA(par)
        W=evaluator.getW(par)
        Pv=sp.diags(evaluator.getPv(par))
        grad=A.transpose()@Pv@W
        return grad



    def getGNDirection(self,par):
        ## for now assuming no constraints and also B=-I
        evaluator=self._evaluator    
        # compute the Gauss Newton search direction
        A=evaluator.getA(par)
        A2=evaluator.getA2(par)
        B=evaluator.getB(par)
        Pv=sp.diags(evaluator.getPv(par))
        W=evaluator.getW(par)
        W2=evaluator.getW2(par)

        ## assemble the GN Normal matrix At * inv( B * inv(P) * Bt ) * A 
        N=A.transpose()@Pv@A
        b=-A.transpose()@Pv@W
        dx=scipy.sparse.linalg.spsolve(N,b)
        return dx


