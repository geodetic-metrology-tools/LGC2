import numpy as np
import scipy as sp

import matplotlib.pyplot as plt
from scipy.sparse import csr_matrix,coo_matrix
from scipy.sparse import diags
import scipy.sparse as sparse

import pyomo as pyo
import pyomo.contrib.incidence_analysis.matching as matching
import pyomo.contrib.incidence_analysis.triangularize as triangularize

import matplotlib.animation as animation

def create_sparse_matrix_with_blocks(block_sizes, density):
    # Create sparse blocks
    blocks = [sparse.random(size,size,1) for size in block_sizes]

    # Combine blocks into a block diagonal sparse matrix
    block_diag_matrix = sparse.block_diag(blocks, format='coo')

    # Total size of the matrix
    total_size = sum(block_sizes)

    # Create upper triangular random matrix
    r_matrix = sparse.triu(sparse.random(total_size, total_size, density=density), k=1)

    # Combine the block diagonal matrix with the random matrix
    result_matrix =(block_diag_matrix.tocoo() + r_matrix.tocoo()).tocoo()

    return result_matrix

def apply_random_permutation(sparse_matrix):
    rows, cols = sparse_matrix.shape

    # Generate random permutations for rows and columns
    row_perm = np.random.permutation(rows)
    col_perm = np.random.permutation(cols)

    # Apply the permutations
    permuted_matrix = sparse_matrix.todense()[row_perm, :][:, col_perm]

    return sparse.coo_matrix(permuted_matrix)


def plot_sparsity_pattern(matrix, title="Sparsity pattern"):
    """
    Plot the sparsity pattern of a matrix.

    Args:
    matrix (np.ndarray): The matrix whose sparsity pattern is to be plotted.
    """
    # Find the non-zero elements
    # Plot the sparsity pattern
    plt.figure(figsize=(8, 6))
    plt.spy(matrix, markersize=1)
    plt.title(title)
    plt.xlabel("Column Index")
    plt.ylabel("Row Index")


class sparsityAnalyzer():
    def __init__(self, sparse_matrix):
        """
        Initialize the class with a sparse SciPy matrix.

        Args:
        sparse_matrix (csr_matrix): A sparse matrix in CSR format.
        """
        self.matrix=sparse_matrix.tocsr()
        # remove explicit zeros
        self.matrix.eliminate_zeros()

        ## compute a maximum matching
        self.maxMatch=matching.maximum_matching(self.matrix)
        ## with the maximum matching, a subset of equations (of equal size as the set of parameters) is identified that allows the full estimation of all parameters
        self.mainEquations=list(self.maxMatch.keys())
        
        
        ## do the block triangular decomposition on the submatrix
        reducedEq,reducedPar = triangularize.block_triangularize(self.matrix[self.mainEquations,:])
        ## create a list of pairs of equation and parameter indices
        ## each parameter list can be determined using the associated equation list + the previous determined parameters
        ## the reordered A matrix has a sparsity pattern that is lower block triangular.
        ## Going block by block, the parameters can be succesively be determined, each time increasing the set of parameters a bit until at the end the whole set of parameters is estimated.
        ## this allows to decompose big problems into a sequence of smaller problems
        eqs=set() 
        pars=set()
        decompositionList=list()
        for newPars,newEqs in zip(reducedPar,reducedEq):
            pars=pars.union(newPars)
            prevEqs=eqs
            eqs=eqs.union(self.getAllAssociatedEqn(pars))
            #eqs=eqs.union(newEqs)
            newEqs=eqs.difference(prevEqs)
            decompositionList.append(tuple((newEqs,newPars)))

        self.decompositionList=decompositionList


    def getMaxMatch(self):
        return self.maxMatch
    def getBTF(self):
        
        ## for square matrices
        eq,par=triangularize.block_triangularize(self.matrix)
        # return as a list of index sets
        return [set(sublist) for sublist in eq],[set(sublist) for sublist in par]
    
    def get_shape(self):
        """
        Return the shape of the sparse matrix.
        """
        return self.matrix.shape
    
    def getAllAssociatedEqn(self, parIdxSet):
        #print("search for rows that only depend on the following indices: ",parIdxSet)
        # for a given set of parameter indices, return all equations that only depend on these parameters
        eqDim=self.matrix.shape[0]
        #tempMat = self.matrix.tocoo()
        ##associatedEqnIdx=set(range(eqDim-1))
        associatedEqnIdx=set()
        for idx in range(eqDim):
            # find eq indices associated to this
            row_non_zero_col_indices = set(self.matrix[idx].nonzero()[1])
            #print("row ",idx, " depends on indices", row_non_zero_col_indices)
            if (row_non_zero_col_indices.issubset(parIdxSet)):
                associatedEqnIdx.add(idx)
                #print("which is a subset of the parameter indices set ", parIdxSet)

        return associatedEqnIdx


def decompose_into_transpositions(perm):
    """
    Decompose a permutation into a sequence of transpositions.

    :param perm: List representing the permutation.
    :return: List of tuples, each tuple is a transposition.
    """
    n = len(perm)
    transpositions = []
    for i in range(n):
        while perm[i] != i:
            swap_idx = perm[i]
            perm[i], perm[swap_idx] = perm[swap_idx], perm[i]
            transpositions.append((i, swap_idx))
    return transpositions

def createSparsityTransformationAnimation(matrix,rowPerm,colPerm):
    matrix=matrix.tocsr()
    # Create the figure for plotting
    fig, ax = plt.subplots()

    # Initialize a list to store the frames for animation
    frames = []

    # Add the original matrix sparsity pattern to frames
    frames.append((ax.spy(matrix, markersize=5),))

    n,m=matrix.shape
    print("rowperm=",rowPerm)
    rowTranspositions= decompose_into_transpositions(rowPerm)
    colTranspositions= decompose_into_transpositions(colPerm)

    rowIndices=[i for i in range(n)]
    frameCounter=0
    # Apply permutations and add each step to frames
    print(rowPerm)
    for trans in reversed(rowTranspositions):
        frameCounter+=1
        i,j=trans
        rowIndices[i],rowIndices[j]=rowIndices[j],rowIndices[i]
        #print(rowIndices)
        # Add the permuted matrix sparsity pattern to frames
        frames.append((ax.spy(matrix[rowIndices,:], markersize=5),))
        #plt.savefig(f"frames/frame_{frameCounter:03d}.png")
        #print(matrix.nnz)
    matrix=matrix[rowIndices,:]
    colIndices=[i for i in range(m)]
    for trans in reversed(colTranspositions):
        frameCounter+=1
        i,j=trans
        colIndices[i],colIndices[j]=colIndices[j],colIndices[i]
        # Add the permuted matrix sparsity pattern to frames
        frames.append((ax.spy(matrix[:,colIndices], markersize=5),))
        #plt.savefig(f"frames/frame_{frameCounter:03d}.png")
        #print(matrix.nnz)


    # Create the animation
    duration=2
    ani = animation.ArtistAnimation(fig, frames, interval=1000*duration/len(frames), blit=True, repeat_delay=5000)

    plt.show()