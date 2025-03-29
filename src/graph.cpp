#include "Graph/graph.hpp"

int main() try {

    graph::graph_t<std::monostate, int> graph;
    std::cin >> graph;

    const auto&[is_bipartite, colors, cycle] = graph.get_bipartite();

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