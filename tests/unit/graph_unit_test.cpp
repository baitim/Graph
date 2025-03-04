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

TEST(Graph_main, test_bipartite_true) {
    graph::graph_t graph1{{1, 2}};
    graph::graph_t graph2{{1, 2}, {1, 3}};
    graph::graph_t graph3{{1, 2}, {1, 3}, {2, 4}};
    graph::graph_t graph4{{1, 2}, {1, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph5{{1, 2}, {2, 3}};
    graph::graph_t graph6{{1, 2}, {2, 3}, {2, 4}};
    graph::graph_t graph7{{1, 2}, {2, 3}, {3, 4}};
    graph::graph_t graph8{{1, 2}, {2, 4}};
    graph::graph_t graph9{{1, 2}, {2, 4}, {3, 4}};
    graph::graph_t graph10{{1, 2}, {3, 4}};
    graph::graph_t graph11{{1, 3}};
    graph::graph_t graph12{{1, 3}, {2, 3}};
    graph::graph_t graph13{{1, 3}, {2, 3}, {2, 4}};
    graph::graph_t graph14{{1, 3}, {2, 3}, {3, 4}};
    graph::graph_t graph15{{1, 3}, {2, 4}};
    graph::graph_t graph16{{1, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph17{{1, 3}, {3, 4}};
    graph::graph_t graph18{{2, 3}};
    graph::graph_t graph19{{2, 3}, {2, 4}};
    graph::graph_t graph20{{2, 3}, {3, 4}};
    graph::graph_t graph21{{2, 4}};
    graph::graph_t graph22{{2, 4}, {3, 4}};
    graph::graph_t graph23{{3, 4}};
    graph::graph_t graph24{{1, 3}, {2, 3}, {2, 4}};

    EXPECT_EQ(std::get<0>(graph1.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph2.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph3.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph4.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph5.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph6.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph7.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph8.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph9.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph10.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph11.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph12.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph13.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph14.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph15.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph16.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph17.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph18.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph19.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph20.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph21.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph22.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph23.get_bipartite()), true);
    EXPECT_EQ(std::get<0>(graph24.get_bipartite()), true);
}

TEST(Graph_main, test_bipartite_false) {
    graph::graph_t graph1{{1, 2}, {1, 3}, {2, 3}};
    graph::graph_t graph2{{1, 2}, {1, 3}, {2, 3}, {2, 4}};
    graph::graph_t graph3{{1, 2}, {1, 3}, {2, 3}, {3, 4}};
    graph::graph_t graph4{{1, 2}, {2, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph5{{1, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph6{{1, 3}, {2, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph7{{2, 3}, {2, 4}, {3, 4}};

    EXPECT_EQ(std::get<0>(graph1.get_bipartite()), false);
    EXPECT_EQ(std::get<0>(graph2.get_bipartite()), false);
    EXPECT_EQ(std::get<0>(graph3.get_bipartite()), false);
    EXPECT_EQ(std::get<0>(graph4.get_bipartite()), false);
    EXPECT_EQ(std::get<0>(graph6.get_bipartite()), false);
    EXPECT_EQ(std::get<0>(graph7.get_bipartite()), false);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}