#ifndef SOLVER_H
#define SOLVER_H

#include "Graph.h"
#include <limits>


const double INF = std::numeric_limits<double>::infinity();

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

struct TabuList {
  int size;
  int **list;
  int itr;
  int tabu_time;
  TabuList(int);
  ~TabuList();
};

struct Move {
  int i, j;
  int u, v;
  double gain;
};

//------------------> Solver Functions

// Constructive
void greedy_constructive_heuristic(Graph&);
void randomize_nearest_neighbor(Graph&, float);
// Local Search
void best_2opt(Graph&, TabuList&);
bool first_2opt(Graph&, int);
bool first_2opt(Graph&, int, TabuList&);
bool first_3opt(Graph&);
// Metaheuristics
void local_search_vnd(Graph&, int ,int);
void tabu_search(Graph&, int, int);
void grasp(Graph&, int);

#endif
