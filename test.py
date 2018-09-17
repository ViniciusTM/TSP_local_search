import os
from math import sqrt

def test(name):
    graph = read_input("EUC_2D/" + name)
    to_int = lambda l: (int(l[0]), int(l[1]))
    f = open("OUT/" + name)

    prob_name = f.readline()[:-1]
    while (prob_name):
        print(prob_name)
        obj = float(f.readline())
        route = [int(i) for i in f.readline().split()]
        result = calc_cost(route, graph)
        prob_name = f.readline()[:-1]

        if len(route) < len(graph):
            print("Not Hamiltonian")
            return 0

        print("Hamiltonian: OK")
        if abs(result - obj) > 0.1:
            print("Path length not matching: {:f} {:f} {:f}".format(abs(result - obj), result, obj))
        else:
            print("Path length: OK")

    f.close()

def read_input(f_name):
    coords = []
    f = open(f_name)
    [f.readline() for i in range(6)]
    while(True):
        line = f.readline().split()
        if line[0] == "EOF":
            break
        coords.append([int(float(i)) for i in line[1:]])
    f.close()

    euc_dist = lambda x,y: int(sqrt((x[0] - y[0])**2 + (x[1] - y [1])**2))
    return [[euc_dist(i,j) for j in coords] for i in coords]


def calc_cost(route, graph):
    result = 0
    current = route[0]
    for next in route[1:]:
        result += graph[current][next]
        current = next

    return result + graph[current][route[0]]


if __name__ == "__main__":
    instances = os.listdir("EUC_2D")

    for name in instances:
        print("=======> Testing " + name)
        test(name)
        print("")
