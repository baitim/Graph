import random
from pathlib import Path

tests_dir = str(Path(__file__).parent)

count_tests = 10

for test_num in range(count_tests) :
    file_name = tests_dir + "/tests_in/test_" + f'{test_num+1:03}' + ".in"
    file = open(file_name, 'w')

    test_str = ""
    file.write(test_str)

    file.close()
    print("test ", test_num + 1, " generated")
