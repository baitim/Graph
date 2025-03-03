#include "Graph/graph.hpp"

int main() {
    try {

        graph::graph_t graph;
        std::cin >> graph;

        const auto&[is_bipartite, colors] = graph.get_bipartite();

        if (!is_bipartite) {
            std::cout << "graph is not bipartite\n";
        } else {
            for (int i = 0, end = colors.size(); i < end; ++i)
                std::cout << i + 1 << " " << (colors[i] ? 'r' : 'b') << " ";
            std::cout << "\n";
        }

    } catch (const graph::error_t& error) {
        return (std::cout << error.what() << "\n", 1);
    }
    return 0;
}