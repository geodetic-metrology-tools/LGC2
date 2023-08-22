#pragma once

#include <list>
#include <queue>
#include <set>
#include <tuple>
#include <stack>
#include <algorithm>
#include <Eigen/Sparse>
#include <Eigen/Dense>
using namespace std;
#define NIL -1
#define INF INT_MAX
class dirGraph;

// A class to represent Bipartite graph for Hopcroft
// Karp implementation oriented at https://www.geeksforgeeks.org/hopcroft-karp-algorithm-for-maximum-matching-set-2-implementation/
class BipGraph
{
    // m and n are number of vertices on left
    // and right sides of Bipartite Graph
    int m, n;

    // adj[u] stores adjacents of left side
    // vertex 'u'. The value of u ranges from 1 to m.
    // 0 is used for dummy vertex
    vector<list<int>> adj;

    // These are basically pointers to arrays needed
    // for hopcroftKarp()
    vector<int> pairU, pairV, dist;


public:
    BipGraph(int m, int n); // Constructor
    BipGraph(Eigen::SparseMatrix<double>& A); // constructor using sparsity pattern
    void addEdge(int u, int v); 

    // Returns true if there is an augmenting path
    bool bfs();

    // Adds augmenting path if there is one beginning
    // with u
    bool dfs(int u);

    // Returns size of maximum matching
    int hopcroftKarp();
    void printMatching();

    // split a set of nodes from the directed graph into the two subsets of nodes of the bipartite graph
    std::pair<set<int>, set<int>> splitEqAndPar(set<int>);
    
    // compute coarse Dulmage-mendelsohn decomposition OUW decomposition: Overdetermined, Underdetermined, Well determined+ Sccs= strongly connected components associated to oriented max matching graph
    std::tuple< std::set<int>, std::set<int>, std::set<int>, vector<std::set<int>>, vector<int>>  BipGraph::getDulmage();
    // get vector of strongly connected components represented as pair of euqation indices and parameter indices.
    // Vector is sorted according to topological order of contracted scc dependencies
    vector<std::pair<std::set<int>, std::set<int>>>  BipGraph::getFineDulmage();

    // return directed graph corresponding to the maximum matching (copy all edges and add also reversed edges for matched original edges)
    // used for Dulmage Mendelsohn decomposition
    dirGraph BipGraph::dirMatch();
   // // split a set of nodes from the directed graph into the two subsets of nodes of the bipartite graph
   // std::pair<set<int>, set<int>> splitEqAndPar(set<int>);

   // // compute coarse Dulmage-mendelsohn decomposition OUW decomposition: Overdetermined, Underdetermined, Well determined+ Sccs= strongly connected components associated to oriented max matching graph
   // std::tuple< std::set<int>, std::set<int>, std::set<int>, std::set<std::set<int>>> getDulmage();
   // void printMatching();
};



class dirGraph {
public :
    dirGraph(int n); // constructor
    void addEdge(int a, int b);
    int inDegree(int j);
    int outDegree(int j);
    // inefficient implementation!!
    std::set<int> dirGraph::findDescendants(int i);
    std::set<int> dirGraph::findAncestors(int i);
    std::set<int> dirGraph::getSourceDescendants();
    std::set<int> dirGraph::getSinkAncestors();
    vector<set<int>> dirGraph::getSCC();
    void dirGraph::printSCCs();
    // topological sorting the vertices.
    // ONLY POSSIBLE IF GRAPH HAS NO CYCLES!
    vector<int> topologicalSort();
    // after maximum matching computation, get the graph that results from contracting the vertices of the scc's
    dirGraph dirGraph::contractSCCgraph();
private:
    // number of vertices
    int n;
    // adj[u] stores adjacents 
    // vertex 'u'. The value of u ranges from 0 to n.
    vector<list<int>> adj;
    vector<list<int>> revAdj;
    vector<set<int>> stronglyConnectedComponents;
    // map vertex->index of associated SCC
    vector<int> vertexToSCC;
    // find strongly connected components using Tarjans algorithm
    void dirGraph::findSCC();
    // A Recursive DFS based function used by SCC()
    void SCCUtil(int u, int& discTime, vector<int>& disc, vector<int>& low, stack<int>& st, vector<bool>& stackMember);
        // A function used by topologicalSort
	void topologicalSortUtil(int v, vector<bool>& visited, stack<int>& stack);
};

void plotSparsity(Eigen::SparseMatrix<double> A, std::vector<int> blockSizes = {});

// input: full rank A matrix (full column rank) output submatrix with same rank but quadrtaic (eliminated rows)
std::vector<int> findFullRankSubMatrix(Eigen::SparseMatrix<double> A);
std::vector<int> getRowOrdering(const Eigen::SparseMatrix<double> &A);

bool isStructuralNonZero(const Eigen::SparseMatrix<double>& A, int row, int col);

Eigen::SparseMatrix<double> maskRows(std::vector<int>, Eigen::SparseMatrix<double>);
Eigen::SparseMatrix<double> maskColumns(std::vector<int>, Eigen::SparseMatrix<double>);

// can be used to get the columns of anothjer matrix by multiplying this from the right
Eigen::SparseMatrix<double> getColumnMaskMatrix(std::vector<int> actInd, int dim);

// can be used to get the rows of another matrix by multiplying this from the left
Eigen::SparseMatrix<double> getRowMaskMatrix(std::vector<int> actInd, int dim);


