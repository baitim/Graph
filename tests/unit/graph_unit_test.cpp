#include "Graph/graph.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

template <typename T>
void assert_vectors_eq(const std::vector<T>& expected, const std::vector<T>& actual) {
    ASSERT_EQ(expected.size(), actual.size());

    for (size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(expected[i], actual[i]) << "at index " << i << '\n';
}

void create_path(int v, std::vector<int>& path) {
    path.push_back(v);
}

std::vector<std::string> get_sorted_files(std::filesystem::path path) {
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(path))
        files.push_back(entry.path().string());

    std::sort(files.begin(), files.end());
    return files;
}

TEST(Graph_shuffle, cmp_ete_with_core) {
    std::string file{__FILE__};
    std::filesystem::path dir = file.substr(0, file.rfind('/'));
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
        const auto&[is_bipartite, colors, cycle] = get_bipartite(graph);
        test_file.close();

        std::ifstream answer_file(answers_str[i]);
        std::string ans_is_bipartite;
        std::getline(answer_file, ans_is_bipartite);
        answer_file.close();

        if (is_bipartite) {
            EXPECT_EQ("graph is bipartite", ans_is_bipartite) << "in test : " << i + 1 << '\n';
        } else {
            EXPECT_EQ("graph is not bipartite", ans_is_bipartite) << "in test : " << i + 1 << '\n';
            EXPECT_EQ(cycle.size() % 2, 1) << "in test : " << i + 1 << '\n';
        }
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

    EXPECT_EQ(get_bipartite(graph1).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph2).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph3).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph4).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph5).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph6).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph7).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph8).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph9).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph10).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph11).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph12).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph13).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph14).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph15).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph16).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph17).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph18).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph19).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph20).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph21).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph22).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph23).is_bipartite, true);
    EXPECT_EQ(get_bipartite(graph24).is_bipartite, true);
}

TEST(Graph_main, test_bipartite_false) {
    graph::graph_t graph1{{1, 2}, {1, 3}, {2, 3}};
    graph::graph_t graph2{{1, 2}, {1, 3}, {2, 3}, {2, 4}};
    graph::graph_t graph3{{1, 2}, {1, 3}, {2, 3}, {3, 4}};
    graph::graph_t graph4{{1, 2}, {2, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph5{{1, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph6{{1, 3}, {2, 3}, {2, 4}, {3, 4}};
    graph::graph_t graph7{{2, 3}, {2, 4}, {3, 4}};

    EXPECT_EQ(get_bipartite(graph1).is_bipartite, false);
    EXPECT_EQ(get_bipartite(graph2).is_bipartite, false);
    EXPECT_EQ(get_bipartite(graph3).is_bipartite, false);
    EXPECT_EQ(get_bipartite(graph4).is_bipartite, false);
    EXPECT_EQ(get_bipartite(graph6).is_bipartite, false);
    EXPECT_EQ(get_bipartite(graph7).is_bipartite, false);

    EXPECT_EQ(get_bipartite(graph1).cycle.size() % 2, 1);
    EXPECT_EQ(get_bipartite(graph2).cycle.size() % 2, 1);
    EXPECT_EQ(get_bipartite(graph3).cycle.size() % 2, 1);
    EXPECT_EQ(get_bipartite(graph4).cycle.size() % 2, 1);
    EXPECT_EQ(get_bipartite(graph6).cycle.size() % 2, 1);
    EXPECT_EQ(get_bipartite(graph7).cycle.size() % 2, 1);
}

TEST(Graph_main, test_bipartite_cycle) {
    graph::graph_t graph1{{1, 2}, {1, 4}, {2, 3}, {3, 6}, {4, 5}, {4, 7}, {5, 6}, {6, 9}, {7, 8}, {8, 9},
                          {5, 9}};
    graph::graph_t graph2{{1, 2}, {1, 4}, {2, 3}, {3, 6}, {4, 5}, {4, 7}, {5, 6}, {6, 9}, {7, 8}, {8, 9},
                          {2, 4}};

    EXPECT_EQ(get_bipartite(graph1).cycle.size() % 2, 1);
    EXPECT_EQ(get_bipartite(graph2).cycle.size() % 2, 1);
}

TEST(Graph_main, test_cycle_self_enclosed_vertex) {
    graph::graph_t graph1{{1, 1}};

    EXPECT_EQ(get_bipartite(graph1).cycle.size(), 3);
}

TEST(Graph_dfs, test_simple_dfs) {
    graph::graph_t graph{{1, 2}, {1, 3}, {2, 4}, {3, 4}};

    std::vector<int> ans;
    graph::graph_t<>::const_iterator_t iter{graph, 0};
    do_dfs(graph, iter, create_path, ans);

    assert_vectors_eq(ans, {1, 3, 2, 0});
}

TEST(Graph_bfs, test_simple_bfs) {
    graph::graph_t graph{{1, 2}, {1, 3}, {2, 4}, {3, 4}};
    
    std::vector<int> ans;
    graph::graph_t<>::const_iterator_t iter{graph, 1};
    do_bfs(graph, iter, create_path, ans);

    assert_vectors_eq(ans, {1, 3, 0 ,2});
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}