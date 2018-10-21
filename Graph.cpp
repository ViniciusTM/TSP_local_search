#include <cmath>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <iostream>
#include "Graph.h"

//    Point
int euclidian_dist(Point a, Point b) {
  return std::round(std::sqrt( std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2) ));
}

int pseudo_euclidian_dist(Point a, Point b) {
  return std::ceil(std::sqrt( (std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2)) / 10.0 ));
}

//    HamiltonianCycle
HamiltonianCycle::HamiltonianCycle(): len(0), valid(false), size(0) { };

void HamiltonianCycle::resize(int n) {
  size = n;
  tour.resize(n);
  positions.resize(n);
}

void HamiltonianCycle::clear() {
  size = 0;
  tour.clear();
  positions.clear();
}

void HamiltonianCycle::add(int id, int position) {
  tour[position] = id;
  positions[id] = position;
}

int HamiltonianCycle::next(int id) {
  int next = (positions[id] + 1) % size;
  return tour[next];
}

int HamiltonianCycle::prev(int id) {
  int prev = (positions[id] - 1 + size) % size;
  return tour[prev];
}

void HamiltonianCycle::swap(int idx1, int idx2) {
  int aux = tour[idx1];
  tour[idx1] = tour[idx2];
  tour[idx2] = aux;

  positions[tour[idx1]] = idx1;
  positions[tour[idx2]] = idx2;
}

void HamiltonianCycle::reverse(int start, int end) {
  int inversion_size = ((size + end - start + 1) % size)/2;
  int left = start, right = end;

  for (int i=0; i<inversion_size; i++) {
    swap(left, right);

    left = (left + 1) % size;
    right = (size + right - 1) % size;
  }
}

bool HamiltonianCycle::between(int a, int b, int c) {
  if (positions[c] > positions[a]) {
    return (positions[b] > positions[a] && positions[b] < positions[c]);
  }
  else {
    return (positions[b] > positions[a] || positions[b] < positions[c]);
  }
}

void HamiltonianCycle::flip(int a, int b, int c, int d) {
  std::vector<int> new_cycle;
  new_cycle.reserve(size);

  new_cycle.push_back(b);

  if (between(b, c, d)) {
    while(prev(b) != d) {
      b = prev(b);
      new_cycle.push_back(b);
    }

    new_cycle.push_back(d);
    new_cycle.push_back(a);

    while(next(a) != c) {
      a = next(a);
      new_cycle.push_back(a);
    }
    new_cycle.push_back(c);
  }

  else {
    while(next(b) != d) {
      b = next(b);
      new_cycle.push_back(b);
    }

    new_cycle.push_back(d);
    new_cycle.push_back(a);

    while(prev(a) != c) {
      a = prev(a);
      new_cycle.push_back(a);
    }
    new_cycle.push_back(c);
  }

  tour = new_cycle;
  for (int i=0; i<size; i++) {
    positions[tour[i]] = i;
  }
}

//  Graph
Graph::Graph(): n(0), built(false) { }

Graph::~Graph() {
  for (int i=0; i<n; i++) {
    delete[] dist_matrix[i];
  }
  delete[] dist_matrix;
}

void Graph::build(const char* f_name, const char* metric, bool neighbor_list) {
  std::ifstream file(f_name);

  // Test if file exists
  if (!file.is_open()) {
    std::cout << "Graph - build:" << std::endl;
    std::cout << "Could not open the file" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Cleaning
  cycle.clear();
  sorted_neighbor.clear();
  if (built) {
    for (int i=0; i<n; i++) {
      delete[] dist_matrix[i];
    }
    delete[] dist_matrix;
  }
  built = true;

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
  dist_matrix = new double*[n];
  for (int i=0; i<n; i++) {
    dist_matrix[i] = new double[n];
  }

  build_dist_matrix(metric, coords);

  // Build sorted neighbor list
  if (neighbor_list) {
    sorted_neighbor.resize(n);
    for (int i=0; i<n; i++) {
      sorted_neighbor[i].resize(n);
    }

    build_neighbor_list();
  }

  // initialize cycle
  cycle.resize(n);
  cycle.len = 0;
  cycle.valid = false;
}

void Graph::build_dist_matrix(const char* metric, std::vector<Point> &coords) {
  std::function<int(Point, Point)> dist_func;

  if (std::strcmp(metric, "euclid") == 0) {
    dist_func = euclidian_dist;
  }
  else if (std::strcmp(metric, "pseudo_euclid") == 0) {
    dist_func = pseudo_euclidian_dist;
  }
  else {
    std::cout << "Invalid distance metric: " << metric << std::endl;
    std::exit(EXIT_FAILURE);
  }

  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      if (i <= j) {
        dist_matrix[i][j] = dist_func(coords[i], coords[j]);
      }
      else {
        dist_matrix[i][j] = dist_matrix[j][i];
      }
    }
  }
}

void Graph::build_neighbor_list() {
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      sorted_neighbor[i][j] = j;
    }
    std::sort(sorted_neighbor[i].begin(), sorted_neighbor[i].end(),
      [&](int a, int b) {return dist_matrix[i][a] < dist_matrix[i][b];});
  }
}
