#ifndef SOLVER_H
#define SOLVER_H

#include <limits>
#include <vector>
#include "Graph.h"

// Global constants and typedefs
const double INF = std::numeric_limits<double>::infinity();
typedef std::array<int, 2> Tuple;

//------------------> Structs
struct Edge {
  int u, v;
  double w;
};

struct Node {
  int id;
  int rank;
  Node *parent;
};
Node* find(Node*);
void merge(Node*, Node*);

struct DisjointSet {
  std::vector<Node> nodes;
  DisjointSet(int);
};

//------------------> Class
class HamiltonianCycle {
public:
  std::vector<int> nodes;
  std::vector<int> positions;

  void reserve(int);
  void clear();
  void push_back(int);
  int size();
  int next(int);
  int prev(int);
  bool between(int, int, int);
  void flip(int, int, int, int);


private:
  int len;
};

class Solver {
public:
  Graph g;
  HamiltonianCycle cycle;
  double obj_func;

  Solver(Graph);
  Solver(const char*, const char*);


  // Constructive Heiristics
  void build_sol_NN(int = 0);
  void build_sol_greedy();
  void build_sol_CW(int = 0);

  // Local Search
  void local_search_2opt(int);

private:
  void greedy_edge_selection(std::vector<Edge>&);
  void make_cycle(std::vector<Edge>&, std::vector<Tuple>&);
};

#endif
