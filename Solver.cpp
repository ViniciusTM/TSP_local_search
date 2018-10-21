#include "Solver.h"
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <random>
#include <chrono>

//    Global constants and typedefs
typedef std::array<int, 2> Tuple;

//    Edge
bool compare_edges(Edge a, Edge b) {
  return (a.w < b.w);
}

//    DisjointSet
Node* find(Node *node) {
  if (node->parent != nullptr) {
    node->parent = find(node->parent);
    return node->parent;
  }
  return node;
}

void merge(Node *n1, Node *n2) {
  Node *head1 = find(n1);
  Node *head2 = find(n2);

  if (head1->rank > head2->rank) {
    head2->parent = head1;
  }
  else if (head1->rank < head2->rank) {
    head1->parent = head2;
  }
  else {
    head1->parent = head2;
    head2->rank++;
  }
}

DisjointSet::DisjointSet(int size) {
  nodes.resize(size);
  for (int i=0; i<size; i++) {
    nodes[i].id = i;
    nodes[i].parent = nullptr;
    nodes[i].rank = 0;
  }
}

//    TabuList
TabuList::TabuList(int size): size(size) {
  list = new int* [size];
  for (int i=0; i<size; i++) {
    list[i] = new int [size];
    for (int j=0; j<size; j++) {
      list[i][j] = 0;
    }
  }
  itr = 0;
}

TabuList::~TabuList() {
  for (int i=0; i<size; i++) {
    delete[] list[i];
  }
  delete[] list;
}

//    Solver

// Constructive
void greedy_constructive_heuristic(Graph &g) {
  int size = g.n*(g.n-1)/2;
  std::vector<Edge> edges(size);
  int idx = 0;

  // Making vector of edges weighted by distance
  for (int i=0; i<g.n; i++) {
    for (int j=i+1; j<g.n; j++) {
      edges[idx].u = i;
      edges[idx].v = j;
      edges[idx].w = g.dist_matrix[i][j];
      idx++;
    }
  }

  // sorting by weight
  std::sort(edges.begin(), edges.end(), compare_edges);

  // Greedy Selection of edges
  Edge e;
  DisjointSet set(g.n);
  std::vector<Tuple> locations(g.n); // for recreating the cycle in O(n)
  std::vector<int> flag(g.n, 0); // 0: not selected; 1: tip; 2: middle
  std::vector<Edge> selected_edges(g.n); // result
  idx = 0;

  for (int i=0; i<size; i++) {
    e = edges[i];
    if (flag[e.u] < 2 && flag[e.v] < 2 && find(&set.nodes[e.v]) != find(&set.nodes[e.u])) {
      merge(&set.nodes[e.v], &set.nodes[e.u]);
      locations[e.u][flag[e.u]] = idx;
      flag[e.u]++;
      locations[e.v][flag[e.v]] = idx;
      flag[e.v]++;
      selected_edges[idx] = e;
      g.cycle.len += g.dist_matrix[e.u][e.v];
      idx++;
    }
    else if (flag[e.u] == 1 && flag[e.v] == 1 && idx == g.n-1) {
      locations[e.u][flag[e.u]] = idx;
      locations[e.v][flag[e.v]] = idx;
      selected_edges[idx] = e;
      g.cycle.len += g.dist_matrix[e.u][e.v];
      break;
    }
  }

  // Making cycle
  int current_idx = 0, current_node = selected_edges[0].v;
  idx = 0;

  g.cycle.add(selected_edges[0].u, idx);
  idx++;
  g.cycle.add(selected_edges[0].v, idx);
  idx++;

  for (int i = 2; i<g.n; i++) {
    if (locations[current_node][0] == current_idx) {
      current_idx = locations[current_node][1];
    }
    else {
      current_idx = locations[current_node][0];
    }

    if (selected_edges[current_idx].u == current_node) {
      current_node = selected_edges[current_idx].v;
    }
    else {
      current_node = selected_edges[current_idx].u;
    }
    g.cycle.add(current_node, idx);
    idx++;
  }

  g.cycle.valid = true;
}

void randomize_nearest_neighbor(Graph &g, float a) {
  std::vector<bool> visited(g.n, false);
  std::vector<int> candidates_list;
  std::vector<int> possibility_list;
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::uniform_int_distribution<int> dist(0, g.n-1);
  int current = dist(generator);

  g.cycle.len = 0;
  g.cycle.add(current, 0);
  candidates_list.reserve(g.n);
  possibility_list.reserve(g.n);

  for (int i=1; i<g.n; i++) {
    double max = 0, min = INF;
    candidates_list.clear();
    possibility_list.clear();
    visited[current] = true;

    // Selecting all possible neighbors
    for (int j=0; j<g.n; j++) {
      if (!visited[j]) {
        candidates_list.push_back(j);
        // Obtaining best neighbor
        if (g.dist_matrix[current][j] > max) {
          max = g.dist_matrix[current][j];
        }
        // Obtaining worst neighbor
        if (g.dist_matrix[current][j] < min) {
          min = g.dist_matrix[current][j];
        }
      }
    }

    //std::cout << "cadidates list: " << candidates_list.size() << std::endl;
    // Creating randomize greeedy options
    for (int idx : candidates_list) {
      if (g.dist_matrix[current][idx] <= min + a * (max - min)) {
        possibility_list.push_back(idx);
      }
    }

    // Random choice
    dist = std::uniform_int_distribution<int>(0, possibility_list.size()-1);
    int chosen = possibility_list[dist(generator)];

    // Adding to solution
    g.cycle.add(chosen, i);
    g.cycle.len += g.dist_matrix[current][chosen];
    current = chosen;
  }

  // Adding distance for closing the cycle
  g.cycle.len += g.dist_matrix[current][g.cycle.tour[0]];
}

// Local Search
void local_search_vnd(Graph &g, int k) {
  if (!g.cycle.valid) {
    std::cout << "Solver - first_2opt: " << std::endl;
    std::cout << "Local search requires a initial solution" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  while (true) {
    if (first_2opt(g, k)) {
      continue;
    }
    else if(first_3opt(g)) {
      continue;
    }
    break;
  }
}

bool first_2opt(Graph &g, int k) {
  char orientation[2] = {'f', 'b'};
  int a, b, c, d;
  int i, j;
  double add, loss;

  for (int idx1=0; idx1<g.n; idx1++) {

    for (char direction : orientation) {
      if (direction == 'f') {
        a = g.cycle.tour[idx1];
        b = g.cycle.next(a);
        i = g.cycle.positions[a];
      }
      else {
        a = g.cycle.tour[idx1];
        b = g.cycle.prev(a);
        i = g.cycle.positions[b];
      }

      for (int idx2=1; idx2<=k; idx2++) {
        c = g.sorted_neighbor[a][idx2];
        if (direction == 'f') {
          j = g.cycle.positions[c];
          d = g.cycle.next(c);
        }
        else {
          d = g.cycle.prev(c);
          j = g.cycle.positions[d];
        }

        if (b == c || d == a) continue;
        if (g.dist_matrix[a][c] > g.dist_matrix[a][b]) break;

        add = g.dist_matrix[a][c] + g.dist_matrix[b][d];
        loss = g.dist_matrix[a][b] + g.dist_matrix[c][d];
        if (loss > add) {
          g.cycle.reverse( (i+1) % g.n, j );
          g.cycle.len = g.cycle.len + add - loss;
          return true;
        }
      }
    }
  }
  return false;
}

bool first_2opt(Graph &g, int k, TabuList &tl) {
  char orientation[2] = {'f', 'b'};
  int a, b, c, d;
  int i, j;
  double add, loss;

  for (int idx1=0; idx1<g.n; idx1++) {

    for (char direction : orientation) {
      if (direction == 'f') {
        a = g.cycle.tour[idx1];
        b = g.cycle.next(a);
        i = g.cycle.positions[a];
      }
      else {
        a = g.cycle.tour[idx1];
        b = g.cycle.prev(a);
        i = g.cycle.positions[b];
      }

      for (int idx2=1; idx2<=k; idx2++) {
        c = g.sorted_neighbor[a][idx2];
        if (direction == 'f') {
          j = g.cycle.positions[c];
          d = g.cycle.next(c);
        }
        else {
          d = g.cycle.prev(c);
          j = g.cycle.positions[d];
        }

        if (b == c || d == a) continue;
        if (g.dist_matrix[a][c] > g.dist_matrix[a][b]) break;

        add = g.dist_matrix[a][c] + g.dist_matrix[b][d];
        loss = g.dist_matrix[a][b] + g.dist_matrix[c][d];
        if (loss > add) {
          g.cycle.reverse( (i+1) % g.n, j );
          g.cycle.len = g.cycle.len + add - loss;
          if (g.dist_matrix[a][b] < g.dist_matrix[c][d]) {
            tl.list[a][b] = tl.itr + tl.tabu_time;
            tl.list[b][a] = tl.itr + tl.tabu_time;
            tl.itr++;
          }
          else {
            tl.list[c][d] = tl.itr + tl.tabu_time;
            tl.list[d][c] = tl.itr + tl.tabu_time;
            tl.itr++;
          }
          return true;
        }
      }
    }
  }
  return false;
}

void best_2opt(Graph &g, TabuList &tl) {
  int a, b, c, d;
  int max_count2;
  Move best;
  double add, loss, gain;
  bool flag = false;

  best.gain = -INF;

  for (int i=0; i<g.n-2; i++) {
    a = g.cycle.tour[i];
    b = g.cycle.next(a);

    if (i == 0) {
      max_count2 = g.n-2;
    }
    else {
      max_count2 = g.n-1;
    }

    for (int j=i+2; j<=max_count2; j++) {
      c = g.cycle.tour[j];
      d = g.cycle.next(c);

      if (tl.list[a][c] > tl.itr || tl.list[b][d] > tl.itr) {
        continue;
      }

      add = g.dist_matrix[a][c] + g.dist_matrix[b][d];
      loss = g.dist_matrix[a][b] + g.dist_matrix[c][d];
      gain = loss - add;

      if (gain > best.gain) {
        flag = true;
        best.i = i;
        best.j = j;
        best.gain = gain;

        if (g.dist_matrix[a][b] < g.dist_matrix[c][d]) {
          best.u = a;
          best.v = b;
        }
        else {
          best.u = c;
          best.v = d;
        }
      }
    }
  }
  if (flag) {
    g.cycle.reverse((best.i + 1)% g.n, best.j);
    g.cycle.len -= best.gain;
    tl.list[best.u][best.v] = tl.itr + tl.tabu_time;
    tl.list[best.v][best.u] = tl.itr + tl.tabu_time;
    tl.itr++;
  }
  else {
    std::cout << "Todos são tabus" << std::endl;
    exit(1);
  }
}

bool first_3opt(Graph &g) {
  int moves_opt3[2] = { 1, 2 };
  int a, b, c, d, e, f;
  int i, j, k;
  double add, loss;

  for (int idx1=0; idx1<g.n; idx1++) {
    i = idx1;
    a = g.cycle.tour[i];
    b = g.cycle.next(a);

    for (int idx2=2; idx2<g.n-2; idx2++) {
      j = (i + idx2) % g.n;
      c = g.cycle.tour[j];
      d = g.cycle.next(c);

      for (int idx3=idx2+2; idx3<g.n; idx3++) {
        k = (i + idx3) % g.n;
        e = g.cycle.tour[k];
        f = g.cycle.next(e);

        for (int move : moves_opt3) {
          if (move == 1) {
            add = g.dist_matrix[a][d] + g.dist_matrix[e][c] + g.dist_matrix[f][b];
          }
          else {
            add = g.dist_matrix[a][d] + g.dist_matrix[e][b] + g.dist_matrix[c][f];
          }
          loss = g.dist_matrix[a][b] + g.dist_matrix[c][d] + g.dist_matrix[e][f];

          if (loss > add) {
            if (move == 1) {
              g.cycle.reverse( (k+1) % g.n, i);
              g.cycle.reverse( (j+1) % g.n, k);
            }
            else {
              g.cycle.reverse( (k+1) % g.n, i);
              g.cycle.reverse( (i+1) % g.n, j);
              g.cycle.reverse( (j+1) % g.n, k);
            }
            g.cycle.len = g.cycle.len + add - loss;
            return true;
          }
        }
      }
    }
  }
  return false;
}

// Metaheuristics
void tabu_search(Graph &g, int k, int max_itr) {
  TabuList tl(g.n);
  std::vector<int> best_tour(g.n);
  double best_of = INF;

  tl.tabu_time = std::ceil(k/1.5);

  // Fist we get to a local optimal using best_2opt
  while(first_2opt(g, k, tl));
  best_tour = g.cycle.tour;
  best_of = g.cycle.len;

  // Try to find a better solution using tabu serach
  for (int itr=0; itr<max_itr; itr++) {
    best_2opt(g, tl);
    if (g.cycle.len < best_of) {
      best_tour = g.cycle.tour;
      best_of = g.cycle.len;
    }
  }

  g.cycle.tour = best_tour;
  g.cycle.len = best_of;
}

void grasp(Graph &g, int max_itr) {
  HamiltonianCycle best_tour;
  best_tour.resize(g.n);
  best_tour.len = INF;

  float a;
  float a_vec[4] = {0.1, 0.3, 0.5, 0.8};
  std::vector<double> probs(4, 1.0/4);
  std::vector<double> mean(4, 0);
  std::vector<int> count(4, 0);

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);

  int idx;
  for (int i=1; i<=max_itr; i++) {
    std::discrete_distribution<int> dist(probs.begin(), probs.end());
    idx = dist(generator);
    a = a_vec[idx];

    randomize_nearest_neighbor(g, a);
    while(first_2opt(g, 20));

    mean[idx] += g.cycle.len;
    count[idx]++;

    if (g.cycle.len < best_tour.len) {
      best_tour.len = g.cycle.len;
      best_tour.tour = g.cycle.tour;
    }

    if (i % 1000 == 0 ) {
      double sum = 0;

      for (int j=0; j<4; j++) {
        mean[j] = pow(best_tour.len/(mean[j]/count[j]), 2);
        sum += mean[j];
      }

      for (int j=0; j<4; j++) {
        probs[j] = mean[j]/sum;
      }

      std::fill(mean.begin(), mean.end(), 0);
      std::fill(count.begin(), count.end(), 0);
    }

  }

  g.cycle.len = best_tour.len;
  for (int i=0; i<g.n; i++) {
    g.cycle.add(best_tour.tour[i], i);
  }
}

// void Solver::build_sol_CW(int start) {
//   std::vector<Edge> savings(g.n*(g.n-1)/2);
//   int idx = 0;
//   obj_func = 0;
//
//   for (int i=0; i<g.n; i++) {
//     for (int j=i+1; j<g.n; j++) {
//       savings[idx].u = i;
//       savings[idx].v = j;
//       savings[idx].w = g.dist_matrix(i,j) - g.dist_matrix(start,i) - g.dist_matrix(start,j);
//       idx++;
//     }
//   }
//   greedy_edge_selection(savings);
// }
//
