#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <dirent.h>
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
  srand(0);


  // solver.build_sol_NN();
  // std::cout << solver.obj_func << std::endl;
  // for (int i=0; i<solver.g.n; i++) {
  //   std::cout << solver.cycle.nodes[i] << " ";
  // }
  // std::cout << std::endl;
  // solver.local_search_2opt(20);
  // std::cout << solver.obj_func << std::endl;



  double build_time, elapsed_time;
  std::string metric;
  std::ofstream f;
  std::clock_t begin;

  for (std::string name : input_names) {
    if (name == "att48.tsp") {
      metric = "epseudo_eclid";
    }
    else {
      metric = "eclid";
    }

    std::cout << "\n==========> SOLVING " + name << std::endl;

    begin = clock();
    Solver solver(("EUC_2D/" + name).c_str(), metric.c_str());
    build_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "Building time: " << build_time << std::endl;

    f.open(("OUT/" + name).c_str());

    begin = clock();
    solver.build_sol_NN();
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> Nearest Neighbor" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << solver.obj_func << std::endl;
    f << "-> Nearest Neighbor" << std::endl;
    f << std::setprecision(7) << solver.obj_func << std::endl;
    for (int j=0; j<solver.g.n; j++) {
      f << solver.cycle.nodes[j] << " ";
    }
    f << std::endl;

    begin = clock();
    solver.local_search_2opt(20);
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> 2-opt" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << solver.obj_func << std::endl;
    f << "-> 2-opt" << std::endl;
    f << std::setprecision(7) << solver.obj_func << std::endl;
    for (int j=0; j<solver.g.n; j++) {
      f << solver.cycle.nodes[j] << " ";
    }
    f << std::endl;

    begin = clock();
    solver.build_sol_greedy();
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> Greedy" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << solver.obj_func << std::endl;
    f << "-> Greedy" << std::endl;
    f << std::setprecision(7) << solver.obj_func << std::endl;
    for (int j=0; j<solver.g.n; j++) {
      f << solver.cycle.nodes[j] << " ";
    }
    f << std::endl;

    begin = clock();
    solver.local_search_2opt(20);
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> 2-opt" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << solver.obj_func << std::endl;
    f << "-> 2-opt" << std::endl;
    f << std::setprecision(7) << solver.obj_func << std::endl;
    for (int j=0; j<solver.g.n; j++) {
      f << solver.cycle.nodes[j] << " ";
    }
    f << std::endl;

    begin = clock();
    solver.build_sol_CW();
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> Clark Wright" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << solver.obj_func << std::endl;
    f << "-> Clark Wright" << std::endl;
    f << std::setprecision(7) << solver.obj_func << std::endl;
    for (int j=0; j<solver.g.n; j++) {
      f << solver.cycle.nodes[j] << " ";
    }
    f << std::endl;

    begin = clock();
    solver.local_search_2opt(20);
    elapsed_time = static_cast<double>(clock() - begin) / CLOCKS_PER_SEC;
    std::cout << "-> 2-opt" << std::endl;
    std::cout << "elapsed time = " << elapsed_time << "s (" << elapsed_time + build_time << ")";
    std::cout << " - Objective Function = " << solver.obj_func << std::endl;
    f << "-> 2-opt" << std::endl;
    f << std::setprecision(7) << solver.obj_func << std::endl;
    for (int j=0; j<solver.g.n; j++) {
      f << solver.cycle.nodes[j] << " ";
    }
    f << std::endl;

    f.close();
  }


}
