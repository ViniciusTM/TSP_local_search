#include "Solver.h"
#include <limits>
#include <array>
#include <vector>
#include <iostream>
#include <algorithm>

//    Global constants and typedefs
const double INF = std::numeric_limits<double>::infinity();
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

//    Solver
void greedy_constructive_heuristic(Graph &g) {
  std::vector<Edge> edges(g.n*(g.n-1)/2);
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

  for (int i=0; i<edges.size(); i++) {
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

void local_search_vnd(Graph &g, int k) {
  if (!g.cycle.valid) {
    std::cout << "Solver - local_search_2opt: " << std::endl;
    std::cout << "Local search requires a initial solution" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  while (true) {
    if (search_2opt(g, k)) {
      continue;
    }
    else if(search_3opt(g)) {
      continue;
    }
    break;
  }
}

bool search_2opt(Graph &g, int k) {
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

bool search_3opt(Graph &g) {
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
// void Solver::build_sol_NN(int start) {
//   std::vector<bool> visited(g.n, false);
//   int current = start;
//   int best_node;
//
//   obj_func = 0;
//   visited[current] = true;
//   cycle.clear();
//   cycle.push_back(current);
//   for (int i=0; i<g.n-1; i++) {
//     double best_cost = INF;
//
//     for (int j=0; j<g.n; j++) {
//       if (!visited[j] && g.dist_matrix(current, j) < best_cost) {
//         best_cost = g.dist_matrix(current, j);
//         best_node = j;
//       }
//     }
//     current = best_node;
//     cycle.push_back(current);
//     visited[current] = true;
//     obj_func += best_cost;
//   }
//
//   obj_func += g.dist_matrix(current, start);
// }
