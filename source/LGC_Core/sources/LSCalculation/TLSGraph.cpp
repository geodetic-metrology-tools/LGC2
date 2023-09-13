#include "TLSGraph.h"
#include <iostream>
#include <iomanip>

using namespace std;
#define NIL -1
#define NIL_hopcroft 0
#define INF INT_MAX


// Constructor
BipGraph::BipGraph(int m, int n):m(m),n(n), adj(m+1), pairU(m+1, NIL_hopcroft), pairV(n+1,NIL_hopcroft), dist(m+1)
{
    //vector<list<int>>adj = new list<int>[m + 1];
}
// second constructor, using constructor delegation
BipGraph::BipGraph(Eigen::SparseMatrix<double>& A) :BipGraph::BipGraph(A.rows(), A.cols())
{
    // creating the edges
    // convention: indices in the graph have to start at 1
    for (int k = 0; k < A.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it) {
            addEdge(it.row() + 1, it.col() + 1);
        }
    }
}

// To add edge from u to v and v to u
void BipGraph::addEdge(int u, int v)
{
    // check if v is already in the list
    auto it = std::find(adj[u].begin(), adj[u].end(), v);
    // only add edge if it does not exist yet.
    if (it == adj[u].end()) {
        adj[u].push_back(v); // Add u to v’s list.
    }
}

// Returns size of maximum matching
int BipGraph::hopcroftKarp()
{
    // pairU[u] stores pair of u in matching where u
    // is a vertex on left side of Bipartite Graph.
    // If u doesn't have any pair, then pairU[u] is NIL_hopcroft
    //vector<int> pairU(m, NIL_hopcroft);

    // pairV[v] stores pair of v in matching. If v
    // doesn't have any pair, then pairU[v] is NIL_hopcroft
    //vector<int>  pairV(n, NIL);

    // dist[u] stores distance of left side vertices
    // dist[u] is one more than dist[u'] if u is next
    // to u'in augmenting path
    //vector<int> dist(m);
    //for (int j = 0; j <= m; j++)
    //    dist[j] = 0;

    // Initialize NIL as pair of all vertices
    for (int u = 0; u <= m; u++)
        pairU[u] = NIL_hopcroft;
    for (int v = 0; v <= n; v++)
        pairV[v] = NIL_hopcroft;

    // Initialize result
    int result = 0;

    // Keep updating the result while there is an
    // augmenting path.
    while (bfs())
    {
        // Find a free vertex
        for (int u = 1; u <= m; u++)

            // If current vertex is free and there is
            // an augmenting path from current vertex
            if (pairU[u] == NIL_hopcroft && dfs(u))
                result++;
    }
    return result;
}


void BipGraph::printMatching()
{
    std::cout << "Printing maximum matching" << std::endl;
    for (int j = 1; j <= m; j++) {
        if (pairU[j] != NIL_hopcroft) {
            std::cout << "Left vertex " << j << " is paired with right vertex " << pairU[j] << std::endl;
        }
    }
}
std::pair<set<int>, set<int>> BipGraph::splitEqAndPar(set<int> nodes)
{
    std::set<int> eqNodes, parNodes;
    // taking into account the numbering of teh bipartite graph (each subset starts numbering at 1)
    for (auto j : nodes) {
        if (j <= m) {
            // eq node
            eqNodes.insert(j);
        }
        else if (j > m) {
            // par node
            parNodes.insert(j - m);
        }
    }

    return std::pair<set<int>, set<int>>(eqNodes, parNodes);
}
std::tuple< std::set<int>, std::set<int>, std::set<int>, vector<std::set<int>>, vector<int>>  BipGraph::getDulmage()
{
    std::set<int> Onodes;
    std::set<int> Unodes;
    // find max matching
    this->hopcroftKarp();
    // create directed graph from max matching
    dirGraph dirG = this->dirMatch();
    // very time consuming methods due to naivest possible implementation
    Onodes = dirG.getSourceDescendants();
    Unodes = dirG.getSinkAncestors();
    std::set<int> OandUnodes = Onodes;
    for (auto j : Unodes) { OandUnodes.insert(j); }

    //Wnodes is the difference of all the nodes and the nodes in O and U
    std::set<int> Wnodes;
    for (int k = 1; k <= n + m; k++) {
        if (OandUnodes.count(k) == 0) {
            Wnodes.insert(k);
        }
    }

    // test fine dulmage decomposition
    vector<std::set<int>> sccs = dirG.getSCC();
    // get the fine ordering
    dirGraph dGOrdering = dirG.contractSCCgraph();
    vector<int> order = dGOrdering.topologicalSort();

    return { Onodes,Unodes,Wnodes,sccs,order };
}

vector<std::pair<std::set<int>, std::set<int>>> BipGraph::getFineDulmage()
{
    // first find a maximum matching equations-parameters with the Hopcroft Karp algorithm
    hopcroftKarp();
    // from this maximum matching create the directed graph with edges from eq to pars + reverse for the matched pairs
    dirGraph dirG = dirMatch();
    // compute the strongly connected components
    vector<std::set<int>> sccs = dirG.getSCC();
    // Compute the Ordering of the components:
    // create new graph from dirG by contracting the vertices inside strongly connected components
    dirGraph dGOrdering = dirG.contractSCCgraph();
    // perform topological ordering of the contracted graph
    vector<int> order = dGOrdering.topologicalSort();
    // split components into eq/par indices
    vector<std::pair<set<int>, set<int>>> sccs_splitted;
    for (auto scc : sccs) {
        sccs_splitted.push_back(splitEqAndPar(scc));
    }
    vector<std::pair<std::set<int>, std::set<int>>> orderedComponents;
    for (int j = 0; j < order.size(); j++) {
        std::set<int> eqComp, parComp;
        int compNumber = order[j];
        eqComp = sccs_splitted[compNumber - 1].first;
        parComp = sccs_splitted[compNumber - 1].second;
        orderedComponents.push_back(std::make_pair(eqComp, parComp));
    }

    return orderedComponents;
}

// Returns true if there is an augmenting path, else returns
// false
bool BipGraph::bfs()
{
    queue<int> Q; //an integer queue

    // First layer of vertices (set distance as 0)
    for (int u = 1; u <= m; u++)
    {
        // If this is a free vertex, add it to queue
        if (pairU[u] == NIL_hopcroft)
        {
            // u is not matched
            dist[u] = 0;
            Q.push(u);
        }

        // Else set distance as infinite so that this vertex
        // is considered next time
        else dist[u] = INF;
    }

    // Initialize distance to NIL as infinite
    dist[NIL_hopcroft] = INF;

    // Q is going to contain vertices of left side only.
    while (!Q.empty())
    {
        // Dequeue a vertex
        int u = Q.front();
        Q.pop();

        // If this node is not NIL and can provide a shorter path to NIL
        if (dist[u] < dist[NIL_hopcroft])
        {
            // Get all adjacent vertices of the dequeued vertex u
            list<int>::iterator i;
            for (i = adj[u].begin(); i != adj[u].end(); ++i)
            {
                int v = *i;

                // If pair of v is not considered so far
                // (v, pairV[V]) is not yet explored edge.
                if (dist[pairV[v]] == INF)
                {
                    // Consider the pair and add it to queue
                    dist[pairV[v]] = dist[u] + 1;
                    Q.push(pairV[v]);
                }
            }
        }
    }

    // If we could come back to NIL using alternating path of distinct
    // vertices then there is an augmenting path
    return (dist[NIL_hopcroft] != INF);
}

// Returns true if there is an augmenting path beginning with free vertex u
bool BipGraph::dfs(int u)
{
    if (u != NIL_hopcroft)
    {
        list<int>::iterator i;
        for (i = adj[u].begin(); i != adj[u].end(); ++i)
        {
            // Adjacent to u
            int v = *i;

            // Follow the distances set by BFS
            if (dist[pairV[v]] == dist[u] + 1)
            {
                // If dfs for pair of v also returns
                // true
                if (dfs(pairV[v]) == true)
                {
                    pairV[v] = u;
                    pairU[u] = v;
                    return true;
                }
            }
        }

        // If there is no augmenting path beginning with u.
        dist[u] = INF;
        return false;
    }
    return true;
}

dirGraph BipGraph::dirMatch()
{
    dirGraph dirG(n + m);
    // go through all edges of the bipartite graph
    for (int startVert = 1; startVert <= m; startVert++) {
        for (auto j : adj[startVert]) {
            // add the directed edge from start to goalVert
            int goalVert = j + m;
            dirG.addEdge(startVert, goalVert);
            // if it is a matching edge in the bipartite graph, add also the reverse direction
            if (pairU[startVert] == j) {
                dirG.addEdge(goalVert, startVert);
            }
        }
    }

    return dirG;
}


dirGraph::dirGraph(int nodes): n(nodes), adj(n+1), revAdj(n+1), vertexToSCC(n+1){
}
// To add edge from u to v and v to u
void dirGraph::addEdge(int u, int v)
{    // check if v is already in the list
    auto it = std::find(adj[u].begin(), adj[u].end(), v);
    // only add edge if it does not exist yet.
    if (it == adj[u].end()) {
        adj[u].push_back(v); // Add u to v’s list.
        //   std::cout << "added directed edge (" << u << "," << v << ")" << std::endl;
        revAdj[v].push_back(u); // add reverse edge
    }
}

int dirGraph::inDegree(int i)
{
    int count = 0;
    for (int j = 1; j <= n; j++) {
        // does the edge j->i exist?
        auto iter = std::find(adj[j].begin(), adj[j].end(), i);
        if (iter != adj[j].end()) {
            count++;
        }
    }
    return count;
}
int dirGraph::outDegree(int i)
{
    int count = 0;
    for (int j = 1; j <= n; j++) {
        // does the edge i->j exist?
        auto iter = std::find(adj[i].begin(), adj[i].end(), j);
        if (iter != adj[i].end()) {
            count++;
        }
    }
    return count;
}

std::set<int> dirGraph::findDescendants(int i) {
    std::set<int> descendants{i};
    int nAdded = 1;
    while (nAdded > 0) {
        std::set<int> newAdded;
        for (auto j : descendants) {
            for (auto k : adj[j]) {
                newAdded.insert(k);
            }
        }
        std::set<int> newSet;
        std::set_union(descendants.begin(), descendants.end(), newAdded.begin(), newAdded.end(), std::inserter(newSet, newSet.begin()));
        nAdded = newSet.size() - descendants.size();
        descendants = newSet;
    }
    return descendants;
}

std::set<int> dirGraph::findAncestors(int i)
{
    std::set<int> ancestors{i};
    int nAdded = 1;
    while (nAdded > 0) {
        std::set<int> newAdded;
        for (auto j : ancestors) {
            for (auto k : revAdj[j]) {
                newAdded.insert(k);
            }
        }
        std::set<int> newSet;
        std::set_union(ancestors.begin(), ancestors.end(), newAdded.begin(), newAdded.end(), std::inserter(newSet, newSet.begin()));
        nAdded = newSet.size() - ancestors.size();
        ancestors = newSet;
    }
    return ancestors;

}

std::set<int> dirGraph::getSourceDescendants()
{
    std::set<int> sourceDescendants;
    for (int j = 1; j <= n; j++) {
        if (this->inDegree(j) == 0) {
            std::set<int> newSourceDescendants = this->findDescendants(j);
            std::set_union(sourceDescendants.begin(), sourceDescendants.end(), newSourceDescendants.begin(), newSourceDescendants.end(), std::inserter(sourceDescendants, sourceDescendants.begin()));
        }
    }
    return sourceDescendants;
}
std::set<int> dirGraph::getSinkAncestors()
{
    std::set<int> sinkAncestors;
    for (int j = 1; j <= n; j++) {
        if (this->outDegree(j) == 0) {
            std::set<int>     newSinkAncestors = this->findAncestors(j);
            std::set_union(sinkAncestors.begin(), sinkAncestors.end(), newSinkAncestors.begin(), newSinkAncestors.end(), std::inserter(sinkAncestors, sinkAncestors.begin()));
        }
    }
    return sinkAncestors;
}

void dirGraph::findSCC()
{
    // reset SCCs
    stronglyConnectedComponents.clear();
    // initialize with value -1, which means vertex does not belong to a strongly connected component with more then one element
    vertexToSCC.assign(n + 1, -1);

    vector<int> disc(n+1);
    vector<int> low(n+1);
    vector<bool> stackMember(n+1);
    stack<int> st;

    // Initialize disc and low, and stackMember arrays
    for (int i = 1; i <= n; i++) {
        disc[i] = NIL;
        low[i] = NIL;
        stackMember[i] = false;
    }

    // Call the recursive helper function to find strongly
    // connected components in DFS tree with vertex 'i'
    int discTime = 0;
    for (int i = 1; i <= n; i++) {
        if (disc[i] == NIL) {
            SCCUtil(i, discTime, disc, low, st, stackMember);
        }
    }
}

vector<set<int>> dirGraph::getSCC()
{
    findSCC();
    return stronglyConnectedComponents;
}

void dirGraph::printSCCs()
{
    // compute SCCs
    findSCC();
    int componentNumber = 0;
    for (auto scc : stronglyConnectedComponents) {
        std::cout << "strongly connected component " << componentNumber << " of size " << scc.size() << ":" << std::endl;
        componentNumber++;
        for (auto vertex : scc) {
            std::cout << vertex << " ";
        }
        std::cout << std::endl;
    }
}

vector<int> dirGraph::topologicalSort()
{
	stack<int> Stack;
	// Mark all the vertices as not visited
    vector<bool> visited(n + 1, false);
//	for (int i = 0; i < V; i++)
//		visited[i] = false;

	// Call the recursive helper function
	// to store Topological
	// Sort starting from all
	// vertices one by one
	for (int i = 1; i <= n; i++)
		if (visited[i] == false)
			topologicalSortUtil(i, visited, Stack);

	// Prepare result = stack
    vector<int> sortedVertices;
	while (Stack.empty() == false) {
		sortedVertices.push_back(Stack.top());
		Stack.pop();
	}
    return sortedVertices;
}

dirGraph dirGraph::contractSCCgraph()
{
    // assuming that the SCC s have been computed and the corresponding members are ready
    // resulting graph will have # SCC vertices
    dirGraph result(stronglyConnectedComponents.size());
    // iterate over all edges and add edge in the result corresponding to the indices of the associated strongly connected components
    for (int start = 1; start <= n; start++) {
        for (auto goal : adj[start]) {
            // indices in directed graph start at 1
            int sccStart = vertexToSCC[start] + 1;
            int sccGoal = vertexToSCC[goal] + 1;
            // check if both vertices are in nontrivial components (>1 elements)
            if (sccStart > 0 && sccGoal > 0) {
                result.addEdge(sccStart, sccGoal);
            }
        }
    }
    return result;
}

void dirGraph::SCCUtil(int u, int& discTime, vector<int>& disc, vector<int>& low, stack<int>& st, vector<bool>& stackMember)
{
    // Initialize discovery time and low value
    disc[u] = low[u] = ++discTime;
    st.push(u);
    stackMember[u] = true;

    // Go through all vertices adjacent to this
    list<int>::iterator i;
    for (i = adj[u].begin(); i != adj[u].end(); ++i) {
        int v = *i; // v is current adjacent of 'u'

        // If v is not visited yet, then recur for it
        if (disc[v] == -1) {
            SCCUtil(v, discTime, disc, low, st, stackMember);

            // Check if the subtree rooted with 'v' has a
            // connection to one of the ancestors of 'u'
            // Case 1 (per above discussion on Disc and Low
            // value)
            low[u] = min(low[u], low[v]);
        }

        // Update low value of 'u' only of 'v' is still in
        // stack (i.e. it's a back edge, not cross edge).
        // Case 2 (per above discussion on Disc and Low
        // value)
        else if (stackMember[v] == true)
            low[u] = min(low[u], disc[v]);
    }

    // head node found, pop the stack and print an SCC
    int w = 0; // To store stack extracted vertices
    if (low[u] == disc[u]) {
        int idxSCC = stronglyConnectedComponents.size();
        set<int> connectedComp;
        while (st.top() != u) {
            w = (int)st.top();
            connectedComp.insert(w);
          //  vertexToSCC[w] = idxSCC;
            //cout << w << " ";
            stackMember[w] = false;
            st.pop();
        }
        w = (int)st.top();
        connectedComp.insert(w);
        //vertexToSCC[w] = idxSCC;
        //cout << w << "\n";
        stackMember[w] = false;
        st.pop();
        // only consider components with more then one vertex. Single vertex components correspond to ummatched equations
        if (connectedComp.size() > 1) {
            stronglyConnectedComponents.push_back(connectedComp);
            // fill vertex->componentIndex map
            for (int idx : connectedComp) { vertexToSCC[idx] = idxSCC; }
        }
    }
}

void dirGraph::topologicalSortUtil(int v, vector<bool>& visited, stack<int>& stack)
{
	// Mark the current node as visited.
	visited[v] = true;

	// Recur for all the vertices
	// adjacent to this vertex
	list<int>::iterator i;
	for (i = adj[v].begin(); i != adj[v].end(); ++i)
		if (!visited[*i])
			topologicalSortUtil(*i, visited, stack);

	// Push current vertex to stack
	// which stores result
	stack.push(v);
}

void plotSparsity(Eigen::SparseMatrix<double> A, std::vector<int> blockSizes) {
	int rows = A.rows();
	int cols = A.cols();
	std::vector<int> separatorIndices;
	int separatorIndex = 0;
	for (auto j:blockSizes){
		separatorIndex += j;
		separatorIndices.push_back(separatorIndex);
    }
	string nonZeroSymbol = "x";
	string zeroSymbol = ".";
	string rowSeparator = "_";
	string colSeparator = "|";
    for (int row = 0; row < rows; row++) {
        // check if we have to insert a separator
		if (std::find(separatorIndices.begin(), separatorIndices.end(), row) != separatorIndices.end())
		{
			for (int k = 0; k < rows + blockSizes.size(); k++)
			{
				std::cout << rowSeparator;
			}
			std::cout << std::endl;
		}
		for (int col = 0; col < cols; col++)
		{
			if (std::find(separatorIndices.begin(), separatorIndices.end(), col) != separatorIndices.end())
			{
				std::cout << colSeparator;
			}
			//if (A.coeffRef(row, col) != 0)
			if (isStructuralNonZero(A,row,col)==true)
			{
				std::cout << nonZeroSymbol;
			}
			else
			{
				std::cout << zeroSymbol;
			}
		}
		std::cout << std::endl;
	}
}

std::vector<int> findFullRankSubMatrix(Eigen::SparseMatrix<double> A)
{
	std::vector<int> rowIndices;
	// transpose matrix and find submatrix with full coulmn  rank. with the transpose we can check more easy if a vector is in the column space
	Eigen::MatrixXd A_dense = A.toDense();
	int colDim = A_dense.cols();
	int rowDim = A_dense.rows();
	int currentRank = 0;
	// plotSparsity(A);
	for (int j = 0; j < rowDim; j++)
	{
		if (currentRank == rowDim)
		{
			break;
		}
		int newRank = A_dense.topRows(j + 1).colPivHouseholderQr().rank();
		if (newRank > currentRank)
		{
			if (newRank > currentRank + 1)
			{
				std::cout << "Rank increased by more then 1 dimension, which should be impossible." << std::endl;
			}
			// rank increased so we add the row
			rowIndices.push_back(j);
			//std::cout << j << std::endl;
			currentRank = newRank;
		}
		// stop if maximum rank is reached
		if (currentRank == colDim)
		{
			break;
		}
	}
	if (currentRank < colDim)
	{
		std::cout << "No full rank submatrix of A could be found. Probably A has not full rank. Undefined behavior during solution process might ocur. Try consistency check." << std::endl;
	}

	return rowIndices;
}
std::vector<int> findFullRankSubMatrixWithQR(Eigen::SparseMatrix<double> A){
    // find full rank submatrix with qr decomposition
	Eigen::SparseMatrix<double> AT = A.transpose();
	//Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> qr;
	Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qr;
	// Eigen makes a decomposition AT * P = Q * R
    // where P is a column permutation matrix. The first rank(A) permutation indices of P correspond to a column submatrix of A with full rank
    qr.compute(AT);
    int n = A.cols();
	std::vector<int> result;
	for (int j = 0; j < n; j++)
	{
		result.push_back(qr.colsPermutation().indices()[j]);
	}
    // sort the result increasing
	std::sort(result.begin(), result.end());
	// for (int j = 0; j < n; j++)
	// {
	// 	std::cout << result[j] << std::endl;
	// }
	if (qr.rank() < n)
	{
		std::cout << "No full rank submatrix of A could be found. Probably A has not full rank. Undefined behavior during solution process might ocur. Try consistency check." << std::endl;
	}
	return result;
}

std::vector<int> getRowOrdering(const Eigen::SparseMatrix<double> &A)
{
    // return vector of indices such that the number of nonzeros are sorted with increasing order
	std::vector<int> nonZerosInRow;
    // create an index vector
	int nRows = A.rows();
	std::vector<int> indexVector;
	for (int j = 0; j < nRows; j++)
	{
		indexVector.push_back(j);
	}
	for (int rowIdx=0; rowIdx<nRows;rowIdx++){
        // count nonzeros
        int nonZeros=0;
		for (int col = 0; col < A.cols(); col++)
		{
			if (isStructuralNonZero(A, rowIdx, col) != 0)
			{
				nonZeros++;
			}
		}
		nonZerosInRow.push_back(nonZeros);
    }
    // sort according to number of nonzeros per row
	std::sort(indexVector.begin(), indexVector.end(), [&nonZerosInRow](int a, int b) { return nonZerosInRow[a] < nonZerosInRow[b]; });

    // // show result
    // for (int j=0;j<nRows;j++){
	// 	std::cout << "(row idx, orig. nnz, sorted nnz)   " << j << " , " << nonZerosInRow[j] << " , " << nonZerosInRow[indexVector[j]] << std::endl;
    // }

	return indexVector;
}

bool isStructuralNonZero(const Eigen::SparseMatrix<double>& A, int row, int col)
{
    // check if (row,col) is an entry of the matrix
    // matrix is column major
    // check all entries of column col
	for (Eigen::SparseMatrix<double>::InnerIterator it(A, col); it; ++it)
	{
		if (it.row() == row)
		{
			return true;
		}
	}

	return false;
}

Eigen::SparseMatrix<double> maskRows(std::vector<int> actRows, Eigen::SparseMatrix<double> mat)
{
	// the cleaner way, multiplication with sparse matrix
	int rowDim = mat.rows();
	return getRowMaskMatrix(actRows, rowDim) * mat;
}

Eigen::SparseMatrix<double> maskColumns(std::vector<int> actCols, Eigen::SparseMatrix<double> mat)
{
	// the cleaner way, multiplication with sparse matrix
	int colDim = mat.cols();
	return mat * getColumnMaskMatrix(actCols, colDim);
}

Eigen::SparseMatrix<double> getColumnMaskMatrix(std::vector<int> actInd, int dim)
{
	int numberIndices = actInd.size();
	Eigen::SparseMatrix<double> result(dim, numberIndices);
	result.setZero();
	for (int col = 0; col < numberIndices; col++)
	{
		result.coeffRef(actInd[col], col) = 1.0;
	}
	return result;
}

Eigen::SparseMatrix<double> getRowMaskMatrix(std::vector<int> actInd, int dim)
{
	int numberIndices = actInd.size();
	Eigen::SparseMatrix<double> result(numberIndices, dim);
	result.setZero();
	for (int row = 0; row < numberIndices; row++)
	{
		result.coeffRef(row, actInd[row]) = 1.0;
	}
	return result;
}
