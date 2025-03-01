import random
from pathlib import Path

tests_dir = str(Path(__file__).parent)

# Если число рёбер m не превышает m_c ≈ (n^(3/2)) / 2,
# то граф с вероятностью ~50% остаётся двудольным.
# Это даёт пороговое соотношение:
# 
#     m / n ≈ sqrt(n) / 2
# 
# Если рёбер меньше, вероятность двудольности выше 50%,
# если больше — ниже.

count_tests = 10
count_vertices = 1000
count_edges = int(count_vertices ** 1.5) // 2
max_weight = count_vertices * 10

for test_num in range(count_tests) :
    file_name = tests_dir + "/tests_in/test_" + f'{test_num+1:03}' + ".in"
    file = open(file_name, 'w')

    used = [[False] * (count_vertices) for _ in range(count_vertices)]

    for _ in range(count_edges):
        while True:
            vertex1 = random.randint(0, count_vertices - 1)
            vertex2 = random.randint(0, count_vertices - 1)

            if vertex1 != vertex2 and not used[vertex1][vertex2]:
                used[vertex1][vertex2] = used[vertex2][vertex1] = True
                weight = random.randint(1, max_weight)
                file.write(f"{vertex1 + 1} -- {vertex2 + 1} {weight}\n")
                break

    file.close()
    print("test ", test_num + 1, " generated")