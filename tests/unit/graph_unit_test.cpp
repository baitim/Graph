#include "Graph/graph.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

std::vector<std::string> get_sorted_files(std::filesystem::path path) {
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(path))
        files.push_back(entry.path().string());

    std::sort(files.begin(), files.end());
    return files;
}

TEST(Graph_shuffle, cmp_ete_with_core) {
    std::string file{__FILE__};
    std::filesystem::path dir = file.substr(0, file.rfind("/"));
    dir = dir / "../end_to_end";

    std::filesystem::path answers_path = dir / "answers_core/";
    std::filesystem::path tests_path   = dir / "tests_in/";

    std::vector<std::string> answers_str = get_sorted_files(answers_path);
    std::vector<std::string> tests_str   = get_sorted_files(tests_path);
    const unsigned count_tests = std::min(answers_str.size(), tests_str.size());

    for (unsigned i = 0; i < count_tests; ++i) {
        std::ifstream test_file(tests_str[i]);
        graph::graph_t<std::monostate, int> graph;
        test_file >> graph;
        const auto&[is_bipartite, colors] = graph.get_bipartite();
        test_file.close();

        std::ifstream answer_file(answers_str[i]);
        std::string ans;
        getline(answer_file, ans);
        answer_file.close();

        EXPECT_EQ(
            is_bipartite ? "graph is bipartite" : "graph is not bipartite",
            ans
        ) << "in test : " << i + 1 << "\n";
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}