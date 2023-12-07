import pyLGC
import os
import csv
import numpy as np
import scipy.sparse as sp
from huberTools import huberSolver,huberSolution
from armijo import armijoSolver,armijoSolution

# seed for random number generation
np.random.seed(0)

def tryGlobalSolveMethods(filename:str):
    ## try to solve from zero knowledge coordinates
    # input: filename
    # output: success status of armijo + sigma
    # + success status of Huber with gamma=2.95

    # create evaluator
    evaluator=pyLGC.LGCController(filename)

    # prepare the initial value
    par=evaluator.getParameter()
    # linearly spaced from 1 to 2
    #initialGuess=100*np.linspace(1,2,par.shape[0])
    # random starting values
    initialGuess=np.random.rand(par.shape[0])

    # armij solver object
    armijoWorker=armijoSolver(evaluator)
    armijoSol=armijoWorker.solve(initialGuess,100)

    # huber solver object
    huberWorker=huberSolver(evaluator)
    huberSol=huberWorker.solve(initialGuess,2.95,100,100000)

    return armijoSol,huberSol

def createResultRow(filename=str,armijoSol=armijoSolution,huberSol=huberSolution):
    parDim=len(armijoSol.primalSolution)
    obsDim=len(armijoSol.residual)

    rowData=[filename,str(parDim),str(obsDim),armijoSol.status,armijoSol.sigma,huberSol.solStatus,huberSol.sigma]
    return rowData

# examples directory
directory = "../plgcTestfiles/"
# file extension
extension=".lgc"

## loop over these files and try to solve from scratch
# write results in csv
header = ["Filename","unkn","obs","Armijo Status", "Armijo Sigma","Huber Status", "Huber Sigma"]
with open('result.csv','w',newline='') as file:
    writer=csv.writer(file)
    writer.writerow(header)
    for filename in os.listdir(directory):
         if filename.endswith(extension):
            filepath = os.path.join(directory, filename)
            armijoSol,huberSol=tryGlobalSolveMethods(filepath)
            row=createResultRow(filename,armijoSol,huberSol)
            writer.writerow(row)






