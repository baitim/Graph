#include "Graph/graph.hpp"

int main() {
    try {
        graph::graph_t graph;
        std::cin >> graph;
        std::cout << graph << "\n";
    } catch (const graph::error_t& error) {
        return (std::cout << error.what() << "\n", 1);
    }
    return 0;
}