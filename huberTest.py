## test performance of QP solvers OSQP and proxQP
## first observation: OSQP has big problems with scaling?? often results in 'infeasible' or trash solutions
## proxQP is much more robust and performs quite well on small examples

import osqp
import proxsuite
import numpy as np
import scipy as sp
from scipy import sparse
from scipy.io import mmread


def loadData():
    # load problem data
    hessian = mmread('build/hessian.mtx')
    gradient = mmread('build/gradient.mtx')
    eqBlock = mmread('build/eqBlock.mtx')
    bEq = mmread('build/beq.mtx')
    ineqBlock = mmread('build/ineqBlock.mtx')
    lbIneq = mmread('build/lbIneq.mtx')
    ubIneq = mmread('build/ubIneq.mtx')
    return [hessian,gradient,eqBlock,bEq,ineqBlock,lbIneq,ubIneq]

def solveOSQP():
    [hessian, gradient, eqBlock, bEq, ineqBlock, lbIneq, ubIneq] = loadData()
    #combine the eq and ineq constraints
    A = sparse.bmat([[eqBlock], [ineqBlock]], format='csc')
    l = np.vstack([bEq, lbIneq])
    u = np.vstack([bEq, ubIneq])
    # Create an OSQP object
    prob = osqp.OSQP()

    # Setup workspace
    prob.setup(hessian, gradient, A, l, u)
    # Solve problem
    res = prob.solve()
    return res.x

def solveProxQP():
    [hessian, gradient, eqBlock, bEq, ineqBlock, lbIneq, ubIneq] = loadData()
    # solve it
    nPar = hessian.shape[1]
    nEq = eqBlock.shape[0]
    nIneq = ineqBlock.shape[0]
    qp = proxsuite.proxqp.dense.QP(nPar, nEq, nIneq)
    qp.init(hessian.toarray(), gradient, eqBlock.toarray(), bEq, ineqBlock.toarray(), lbIneq, ubIneq)
    qp.settings.verbose = True
    qp.solve()
    return qp.results.x


x_OSQP = solveOSQP()
x_proxQP = solveProxQP()
print(x_OSQP)
print(x_proxQP)
print("difference=",np.linalg.norm(x_OSQP-x_proxQP))
