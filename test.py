import os
from math import sqrt, ceil

def test(name):
    graph = read_input("EUC_2D/" + name)
    to_int = lambda l: (int(l[0]), int(l[1]))
    f = open("OUT/" + name)

    prob_name = f.readline()[:-1]
    while (prob_name):
        print(prob_name)
        obj = float(f.readline())
        route = [int(i) for i in f.readline().split()]

        if (hamiltonian_check(route, len(graph))):
            print("Hamiltonian Cycle: OK")
        else:
            print("Hamiltonian Cycle: Fail")

        route_len = calc_cost(route, graph)
        diff = abs(route_len - obj)
        if (diff < 0.1):
            print("Route length: OK")
        else:
            print("Route length: Fail - reported: {:.1f}  real:{:.1f}  diff:{:.1f}".format(obj, route_len, diff))

        f.readline()
        prob_name = f.readline()[:-1]

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
    pseudo_euc_dist = lambda x,y: ceil(sqrt( ((x[0] - y[0])**2 + (x[1] - y [1])**2)/10 ))

    if (f_name[7:] == "att48.tsp"):
        return [[pseudo_euc_dist(i,j) for j in coords] for i in coords]

    return [[euc_dist(i,j) for j in coords] for i in coords]


def calc_cost(route, graph):
    result = 0
    current = route[0]
    for next in route[1:]:
        result += graph[current][next]
        current = next

    return result + graph[current][route[0]]

def hamiltonian_check(route, size):
    if len(route) != size:
        return False

    if sorted(route) != list(range(size)):
        return False

    return True


if __name__ == "__main__":
    instances = sorted(os.listdir("EUC_2D"))

    for name in instances:
        print("=======> Testing " + name)
        test(name)
        print("")
