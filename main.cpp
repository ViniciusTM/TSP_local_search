#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <dirent.h>
#include <algorithm>
#include "Graph.h"
#include "Solver.h"

int main() {
  std::vector<std::string> input_names;

  DIR *dir = opendir("EUC_2D");
  struct dirent *dp;
  while ((dp = readdir(dir)) != nullptr) {
    if (dp->d_name[0] != '.') {
      input_names.push_back(dp->d_name);
    }
  }
  closedir(dir);
  std::sort(input_names.begin(), input_names.end());

  double build_time, elapsed_time;
  std::string metric;
  std::ofstream f;
  std::clock_t begin;
  Graph graph;

  // graph.build("EUC_2D/att48.tsp");
  // greedy_constructive_heuristic(graph);
  // std::cout << "obj func: " << graph.cycle.len << std::endl;
  // for (int i=0; i<graph.n; i++) {
  //   std::cout << graph.cycle.tour[i] << " ";
  // }
  // std::cout << std::endl;
  //
  // local_search_3opt(graph);
  // std::cout << "obj func: " << graph.cycle.len << std::endl;
  // for (int i=0; i<graph.n; i++) {
  //   std::cout << graph.cycle.tour[i] << " ";
  // }
  // std::cout << std::endl;

  for (std::string name : input_names) {
    if (name == "att48.tsp") {
      metric = "pseudo_euclid";
    }
    else {
      metric = "euclid";
    }

    std::cout << "\n==========> SOLVING " + name << std::endl;

    begin = clock();
    graph.build(("EUC_2D/" + name).c_str(), metric.c_str());
    build_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "Building time: " << build_time << std::endl;

    f.open(("OUT/" + name).c_str());

    begin = clock();
    greedy_constructive_heuristic(graph);
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> Constructive Heuristic" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << graph.cycle.len << std::endl;
    f << "-> Constructive Heuristic" << std::endl;
    f << std::setprecision(7) << graph.cycle.len << std::endl;
    for (int j=0; j<graph.n; j++) {
      f << graph.cycle.tour[j] << " ";
    }
    f << std::endl;
    f << std::endl;

    begin = clock();
    local_search_vnd(graph, 20);
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> Variable Neighborhood Search [2opt (k=20) + 3opt]" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << graph.cycle.len << std::endl;
    f << "-> Variable Neighborhood Search [2opt (k=20) + 3opt]" << std::endl;
    f << std::setprecision(7) << graph.cycle.len << std::endl;
    for (int j=0; j<graph.n; j++) {
      f << graph.cycle.tour[j] << " ";
    }
    f << std::endl;

    f.close();
  }
}
