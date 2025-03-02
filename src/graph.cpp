#include "Graph/graph.hpp"

int main() {
    try {

        graph::graph_t graph;
        std::cin >> graph;

        std::vector<int> colors = graph.get_bipartite();
        for (int i = 0, end = colors.size(); i < end; ++i)
            std::cout << i + 1 << " " << (colors[i] ? 'r' : 'b') << " ";
        std::cout << "\n";

    } catch (const graph::error_t& error) {
        return (std::cout << error.what() << "\n", 1);
    }
    return 0;
}