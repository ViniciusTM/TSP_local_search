#include "Solver.h"
#include <vector>
#include <tuple>
#include "Graph.h"

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

//    HamiltonianCycle
void HamiltonianCycle::reserve(int n) {
  nodes.reserve(n);
  positions.resize(n);
}

void HamiltonianCycle::clear() {
  len = 0;
  nodes.clear();
  positions.clear();
}

void HamiltonianCycle::push_back(int id) {
  nodes.push_back(id);
  positions[id] = len;
  len++;
}

int HamiltonianCycle::size() {
  return len;
}

int HamiltonianCycle::next(int id) {
  int next = (positions[id] + 1) % len;
  return nodes[next];
}

int HamiltonianCycle::prev(int id) {
  int prev = (positions[id] - 1 + len) % len;
  return nodes[prev];
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
  new_cycle.reserve(len);

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

  nodes = new_cycle;
  for (int i=0; i<len; i++) {
    positions[nodes[i]] = i;
  }
}

//    Solver
Solver::Solver(Graph g): g(g) {
  cycle.clear();
  cycle.reserve(g.n);
}

Solver::Solver(const char* f_name, const char* metric): g(Graph(f_name, metric)) {
  cycle.clear();
  cycle.reserve(g.n);
}

void Solver::greedy_edge_selection(std::vector<Edge> &edges) {
  int idx = 0;
  Edge e;
  DisjointSet set(g.n);
  std::vector<int> flag(g.n, 0); // 0 -> not in the cycle; 1-> one of the tips; 2-> in the middle
  std::vector<Tuple> locations(g.n); // for recreating the cycle in O(n)
  std::vector<Edge> sol(g.n);

  std::sort(edges.begin(), edges.end(), compare_edges);
  for (int i=0; i<edges.size(); i++) {
    e = edges[i];
    if (flag[e.u] < 2 && flag[e.v] < 2 && find(&set.nodes[e.v]) != find(&set.nodes[e.u])) {
      merge(&set.nodes[e.v], &set.nodes[e.u]);
      locations[e.u][flag[e.u]] = idx;
      flag[e.u]++;
      locations[e.v][flag[e.v]] = idx;
      flag[e.v]++;
      sol[idx] = e;
      obj_func += g.dist_matrix(e.u, e.v);
      idx++;
    }
    else if (flag[e.u] == 1 && flag[e.v] == 1 && idx == g.n-1) {
      locations[e.u][flag[e.u]] = idx;
      locations[e.v][flag[e.v]] = idx;
      sol[idx] = e;
      obj_func += g.dist_matrix(e.u,e.v);
      break;
    }
  }

  make_cycle(sol, locations);
}

void Solver::make_cycle(std::vector<Edge> &sol, std::vector<Tuple> &locations) {
  int current_node = sol[0].v, current_idx = 0;

  cycle.clear();
  cycle.push_back(sol[0].u);
  cycle.push_back(sol[0].v);

  int count = 0;

  while(cycle.size() < g.n) {
    if (locations[current_node][0] == current_idx) {
      current_idx = locations[current_node][1];
    }
    else {
      current_idx = locations[current_node][0];
    }

    if (sol[current_idx].u == current_node) {
      current_node = sol[current_idx].v;
    }
    else {
      current_node = sol[current_idx].u;
    }
    cycle.push_back(current_node);
  }

}

void Solver::build_sol_NN(int start) {
  std::vector<bool> visited(g.n, false);
  int current = start;
  int best_node;

  obj_func = 0;
  visited[current] = true;
  cycle.clear();
  cycle.push_back(current);
  for (int i=0; i<g.n-1; i++) {
    double best_cost = INF;

    for (int j=0; j<g.n; j++) {
      if (!visited[j] && g.dist_matrix(current, j) < best_cost) {
        best_cost = g.dist_matrix(current, j);
        best_node = j;
      }
    }
    current = best_node;
    cycle.push_back(current);
    visited[current] = true;
    obj_func += best_cost;
  }

  obj_func += g.dist_matrix(current, start);
}

void Solver::build_sol_greedy() {
  std::vector<Edge> edges(g.n*(g.n-1)/2);
  int idx = 0;
  obj_func = 0;

  for (int i=0; i<g.n; i++) {
    for (int j=i+1; j<g.n; j++) {
      edges[idx].u = i;
      edges[idx].v = j;
      edges[idx].w = g.dist_matrix(i,j);
      idx++;
    }
  }
  greedy_edge_selection(edges);
}

void Solver::build_sol_CW(int start) {
  std::vector<Edge> savings(g.n*(g.n-1)/2);
  int idx = 0;
  obj_func = 0;

  for (int i=0; i<g.n; i++) {
    for (int j=i+1; j<g.n; j++) {
      savings[idx].u = i;
      savings[idx].v = j;
      savings[idx].w = g.dist_matrix(i,j) - g.dist_matrix(start,i) - g.dist_matrix(start,j);
      idx++;
    }
  }
  greedy_edge_selection(savings);
}

void Solver::local_search_2opt(int k) {
  int b, c, d;
  double gain, loss;
  bool flag = false, end = false;

  int count = 0;
  while(!end) {
    end = true;
    flag = false;

    for (int a=0; a<g.n; a++) {
      b = cycle.next(a);

      for (int i=1; i<k; i++) {
        c = g.sorted_neighbor(b, i);

        if (g.dist_matrix(a, b) > g.dist_matrix(b, c)) {
          d = cycle.prev(c);
          //if (d == a || d == b) {continue;}

          gain = g.dist_matrix(b, c) + g.dist_matrix(a, d);
          loss = g.dist_matrix(a, b) + g.dist_matrix(c, d);

          if (loss > gain) {
            //std::cout << a << " " << b << " " << c << " " << d << std::endl;
            cycle.flip(a, b, c, d);
            //std::cout << std::endl;
            obj_func = obj_func - loss + gain;
            flag = true;
            end = false;
            break;
          }
        }

      }
      if (flag) {break;}
    }
    count++;
    if (count > 100) {break;}
  }

}
