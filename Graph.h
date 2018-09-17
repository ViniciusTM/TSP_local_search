#ifndef GRAPH_H
#define GRAPH_H
#include <limits>
#include <boost/numeric/ublas/matrix.hpp>

using namespace boost::numeric::ublas;

// -----------------> Structs
struct Point {
  float x, y;
};
int euclidian_dist(Point, Point);
int pseudo_euclidian_dist(Point, Point);

// -----------------> Graph Class
class Graph {
public:
  int n;                        // Number of vertices
  matrix<double> dist_matrix;   // Adjacency matrix
  matrix<int> sorted_neighbor;  // Sorted list of Neighbor for each node

  Graph(const char*, const char* = "euclid");
  void build(const char*, const char* = "euclid");

private:
  void build_dist_matrix(const char*, std::vector<Point>&);
};

#endif
