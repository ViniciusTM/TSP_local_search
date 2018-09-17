#include <cmath>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <boost/numeric/ublas/matrix.hpp>
#include "Graph.h"

using namespace boost::numeric::ublas;

//    Point
int euclidian_dist(Point a, Point b) {
  return int(std::sqrt( std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) ));
}

int pseudo_euclidian_dist(Point a, Point b) {
  return std::ceil(std::sqrt( (std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2)) / 10.0 ));
}

//    Graph
Graph::Graph(const char* f_name, const char* metric) {
  build(f_name, metric);
}

void Graph::build(const char* f_name, const char* metric) {
  std::ifstream file(f_name);

  // Test if file exists
  if (!file.is_open()) {
    std::cout << "Could not open the file" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Reading the header and number of vertices
  std::string aux;

  for (int i=0; i<4; i++) std::getline(file, aux); // Reding first 4 header lines
  std::stringstream ss(aux.substr(11));
  ss >> n;                                          // getting the number of vertices
  for (int i=0; i<2; i++) std::getline(file, aux); // Reading last 2 header lines

  // Reading Coordinates
  int id;
  std::vector<Point> coords(n);

  for (int i=0; i<n; i++) {
    file >> id >> coords[i].x >> coords[i].y;
  }
  file.close();

  // Building distance matrix
  build_dist_matrix(metric, coords);
}

void Graph::build_dist_matrix(const char* metric, std::vector<Point>& coords) {
  std::function<int(Point, Point)> dist_func;

  if (std::strcmp(metric, "euclid")) {
    dist_func = euclidian_dist;
  }
  else if (std::strcmp(metric, "pseudo_eclid")) {
    dist_func = pseudo_euclidian_dist;
  }
  else {
    std::cout << "Invalid distance metric " << metric << std::endl;
    std::exit(EXIT_FAILURE);
  }

  dist_matrix.resize(n, n);

  sorted_neighbor.resize(n, n);
  matrix<int>::iterator1 itr = sorted_neighbor.begin1();

  std::vector<int> dist_vec(n);
  auto comparator = [&dist_vec](int a, int b){ return dist_vec[a] < dist_vec[b]; };

  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      if (i < j) {
        dist_matrix(i,j) = dist_func(coords[i], coords[j]);
      }
      else {
        dist_matrix(i,j) = dist_matrix(j,i);
      }
      dist_vec[j] = dist_matrix(i,j);
      sorted_neighbor(i,j) = j;
    }
    std::sort(itr.begin(), itr.end(), comparator);
    itr++;
  }
}
