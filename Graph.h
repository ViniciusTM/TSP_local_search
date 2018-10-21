#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

// -----------------> Structs
struct Point {
  float x, y;
};
int euclidian_dist(Point, Point);
int pseudo_euclidian_dist(Point, Point);

//------------------> Class HamiltonianCycle
class HamiltonianCycle {
public:
  double len;
  bool valid;
  std::vector<int> tour;
  std::vector<int> positions;

  HamiltonianCycle();
  void resize(int);
  void clear();
  void add(int, int);
  void swap(int, int);
  void reverse(int, int);
  int next(int);
  int prev(int);

  bool between(int, int, int);
  void flip(int, int, int, int);


private:
  int size;
};

// -----------------> Graph Class
class Graph {
public:
  int n;                                          // Number of vertices
  double **dist_matrix;                           // Adjacency matrix
  std::vector<std::vector<int> > sorted_neighbor; // Sorted list of Neighbor for each node
  HamiltonianCycle cycle;                         // Current solution of the TSP

  Graph();
  ~Graph();
  void build(const char*, const char* = "euclid", bool = false);

private:
  bool built;
  void build_dist_matrix(const char*, std::vector<Point>&);
  void build_neighbor_list();
};

#endif
