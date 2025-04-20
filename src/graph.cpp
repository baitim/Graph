#include "Graph/graph.hpp"

#if defined(WITH_DFS) || defined(WITH_BFS)
void print_int(int i, std::ostream& os) {
    os << i << "\n";
}
#endif

int main() try {
    using Graph = graph::graph_t<std::monostate, int>;
    Graph graph;
    std::cin >> graph;

#if defined(WITH_DFS) || defined(WITH_BFS)
    Graph::const_iterator_t iter{graph, 0};
    #ifdef WITH_BFS
        do_bfs(graph, iter, print_int, std::cout);
    #else
        do_dfs(graph, iter, print_int, std::cout);
    #endif
#endif

    auto&& [is_bipartite, colors, cycle] = get_bipartite(graph);

    if (!is_bipartite) {
        std::cout << "graph is not bipartite, odd cycle:\n";
        for (int i = 0, end = cycle.size(); i < end; ++i)
            std::cout << cycle[i] + 1 << " ";
        std::cout << '\n';
    } else {
        for (int i = 0, end = colors.size(); i < end; ++i)
            std::cout << i + 1 << " " << (colors[i] ? 'r' : 'b') << " ";
        std::cout << '\n';
    }

} catch (const graph::error_t& error) {
    std::cout << error.what() << '\n';
    return 1;
} catch (...) {
    std::cout << print_red("Unknown error\n");
    return 1;
}