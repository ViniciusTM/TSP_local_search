#ifndef SOLVER_H
#define SOLVER_H

#include "Graph.h"

//------------------> Solver Functions
void greedy_constructive_heuristic(Graph&);
void local_search_vnd(Graph&, int);

bool search_2opt(Graph&, int);
bool search_3opt(Graph&);

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

#endif
