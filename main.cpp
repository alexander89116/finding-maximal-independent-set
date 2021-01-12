#include <iostream>

#include "finding-maximal-independent-set.hpp"

int main() {
    BOOST_LOG_TRIVIAL(info) << "Read graph";
    int vertices_count, edges_count;
    std::cin >> vertices_count >> edges_count;
    int all_vertices = (1 << vertices_count) - 1;
    Graph graph;

    graph.edges.resize(vertices_count);
    while (edges_count--) {
        int from, to;
        std::cin >> from >> to;
        graph.edges[from].insert(to);
        graph.edges[to].insert(from);
    }
    std::cout << FindMaxIndependentSubGraph(SubGraph{all_vertices, graph}) << std::endl;
    return 0;
}
