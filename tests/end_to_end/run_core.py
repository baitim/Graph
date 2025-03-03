import glob
import networkx as nx
from pathlib import Path

ete_tests_dir = str(Path(__file__).parent)

tests_dir = ete_tests_dir + "/tests_in"
answers_dir = ete_tests_dir + "/answers_core"
test_num = 0
tests_files = list(map(str, glob.glob(tests_dir + "/test_*.in")))
tests_files.sort()

for test_num in range(len(tests_files)):
    test_file_name = tests_dir + "/test_" + f'{test_num+1:03}' + ".in"
    test_data = open(test_file_name, "r")

    G = nx.Graph()
    for line in test_data:
        parts = line.strip().split(",")
        edge, weight = parts[0], int(parts[1])
        v1, v2 = map(int, edge.split(" -- "))

        G.add_edge(v1, v2, weight=weight)

    test_data.close()

    answer_file_name = answers_dir + "/" + f'{test_num+1:03}' + ".ans"
    answer_file = open(answer_file_name, 'w')

    is_bipartite = nx.is_bipartite(G)
    if is_bipartite:
        answer_file.write("graph is bipartite")
        # color_map = nx.bipartite.color(G)
        # print(" ".join(f"{node} {'b' if color else 'r'}" for node, color in color_map.items()))
    else:
        answer_file.write("graph is not bipartite")

    answer_file.close()
    print("test ", test_num + 1, " processed")