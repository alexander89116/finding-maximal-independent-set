#include <bitset>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

#include <boost/log/trivial.hpp>

#include "subgraph.hpp"

int FindMaxIndependentSubGraph(const SubGraph &graph);

int FindMaxIndependentSetInConnectedSubGraph(const SubGraph &graph) {
    if (graph.VerticesCount() < 2) {
        return 1;
    }
    auto degrees = graph.GetVerticesDegrees();
    std::sort(degrees.begin(), degrees.end());
    auto[degree, vertex] = degrees.front();
    auto children = graph.GetChildren(vertex);
    int set = ~GetVertexId(vertex) & ~GetVertexSetFromVector(children);
    // 1
    if (degree == 1) {
        int to = graph.GetChildren(vertex).front();
        return 1 + FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(vertex) & ~GetVertexId(to)));
    }
    // 2
    if (degree == 2) {
        // 2.1
        if (degrees.back().first == 2) {
            return graph.VerticesCount() / 2;
        }
        // 2.2
        if (graph.graph.HasEdge(children.front(), children.back())) {
            return 1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set));
        }
        // 2.3
        return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                        2 + FindMaxIndependentSubGraph(
                                graph.GetSubGraph(~GetVertexSetFromVector(graph.GetChildren(children.front())) &
                                                  ~GetVertexSetFromVector(graph.GetChildren(children.back())))));

    }
    // 3
    if (degree == 3) {
        // 3.1
        if (graph.graph.HasEdge(children[0], children[1]) &&
            graph.graph.HasEdge(children[1], children[2]) &&
            graph.graph.HasEdge(children[2], children[0])) {
            return 1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set));
        }
        // 3.2
        for (int i = 0; i < 3; i++) {
            if (graph.graph.HasEdge(children[i], children[(i + 1) % 3]) &&
                graph.graph.HasEdge(children[i], children[(i + 2) % 2])) {
                return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                2 + FindMaxIndependentSubGraph(
                                        graph.GetSubGraph(
                                                ~GetVertexSetFromVector(graph.GetChildren(children[(i + 1) % 3])) &
                                                ~GetVertexSetFromVector(graph.GetChildren(children[(i + 2) % 3])))));
            }
        }
        // 3.3
        std::vector<VertexSet> a(3);
        for (int i = 0; i < 3; i++) {
            a[i] = graph.set & ~GetVertexSetFromVector(children) &
                   ~GetVertexSetFromVector(graph.GetChildren(children[i]));
        }
        VertexSet all_a = a[0] & a[1] & a[2];

        for (int i = 0; i < 3; i++) {
            if (graph.graph.HasEdge(children[i], children[(i + 1) % 3])) {
                // 3.3.1
                if ((VerticesCount(a[i] & a[(i + 2) % 3]) <= VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) &&
                     VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) == graph.VerticesCount() - 6) ||
                    (VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) <= VerticesCount(a[i] & a[(i + 2) % 3]) &&
                     VerticesCount(a[i] & a[(i + 2) % 3]) == graph.VerticesCount() - 6)) {
                    return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                    2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[(i + 2) % 3])));
                }
                // 3.3.2
                return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 2) % 3])),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[(i + 1) % 3] & a[(i + 2) % 3])),});
            }
        }
        // 3.4
        // 3.4.1
        if (VerticesCount(all_a) >= graph.VerticesCount() - 7) {
            return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                            3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a)));
        }
        // 3.4.2
        if (VerticesCount(all_a) == graph.VerticesCount() - 8 ||
            VerticesCount(all_a) == graph.VerticesCount() - 9) {
            // 3.4.2.2
            for (int i = 0; i < 3; i++) {
                if (VerticesCount(a[i] & a[(i + 1) % 3]) >= VerticesCount(all_a) + 2) {
                    return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                     2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 1) % 3])),
                                     3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
                }
            }
            // 3.4.2.1
            return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                            3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a)));
        }
        // 3.4.3
        // 3.4.3.1
        if (VerticesCount(a[0] & a[1]) <= VerticesCount(all_a) + 1 &&
            VerticesCount(a[1] & a[2]) <= VerticesCount(all_a) + 1 &&
            VerticesCount(a[2] & a[0]) <= VerticesCount(all_a) + 1) {
            return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                            3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a)));
        }
        // 3.4.3.2
        for (int i = 0; i < 3; i++) {
            if (VerticesCount(a[i] & a[(i + 1) % 3]) >= VerticesCount(all_a) + 2 &&
                VerticesCount(a[i] & a[(i + 2) % 3]) <= VerticesCount(all_a) + 1 &&
                VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) <= VerticesCount(all_a) + 1) {
                return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 1) % 3])),
                                 3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
            }
        }
        // 3.4.3.3
        for (int i = 0; i < 3; i++) {
            if (VerticesCount(a[i] & a[(i + 1) % 3]) >= VerticesCount(all_a) + 2 &&
                VerticesCount(a[i] & a[(i + 2) % 3]) >= VerticesCount(all_a) + 2 &&
                VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) <= VerticesCount(all_a) + 1) {
                return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 1) % 3])),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 2) % 3])),
                                 3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
            }
        }
        // 3.4.3.4
        std::vector<std::pair<Vertex, Vertex>> u(3);
        std::vector<VertexSet> Au(3);
        for (int i = 0; i < 3; i++) {
            u[i] = graph.GetSubGraph(a[i] & a[(i + 1) % 3] & ~a[(i + 2) % 3]).GetPair();
            Au[i] = ~GetVertexSetFromVector(graph.GetChildren(u[i].first)) &
                    ~GetVertexSetFromVector(graph.GetChildren(u[i].second));
        }
        // 3.4.3.4.1
        for (int i = 0; i < 3; i++) {
            if (VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) == VerticesCount(all_a) + 2 &&
                graph.graph.HasEdge(u[i].first, u[i].second)) {
                return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 1) % 3])),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 2) % 3])),
                                 3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
            }
        }
        // 3.4.3.4.2
        if (VerticesCount(a[0] & a[1]) == VerticesCount(all_a) + 2 &&
            VerticesCount(a[1] & a[2]) == VerticesCount(all_a) + 2 &&
            VerticesCount(a[2] & a[0]) == VerticesCount(all_a) + 2) {
            return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                             4 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a & Au[0])),
                             4 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a & Au[1])),
                             4 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a & Au[2])),
                             3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
        }
        // 3.4.3.4.3
        for (int i = 0; i < 3; i++) {
            if (VerticesCount(a[i] & a[(i + 1) % 3]) == VerticesCount(all_a) + 2 &&
                VerticesCount(a[i] & a[(i + 2) % 3]) == VerticesCount(all_a) + 2 &&
                VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) != VerticesCount(all_a) + 2) {
                return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                 4 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a & Au[(i + 1) % 3])),
                                 4 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a & Au[(i + 2) % 3])),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[(i + 1) % 3] & a[(i + 2) % 3])),
                                 3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
            }
        }
        // 3.4.3.4.4
        for (int i = 0; i < 3; i++) {
            if (VerticesCount(a[i] & a[(i + 1) % 3]) != VerticesCount(all_a) + 2 &&
                VerticesCount(a[i] & a[(i + 2) % 3]) != VerticesCount(all_a) + 2 &&
                VerticesCount(a[(i + 1) % 3] & a[(i + 2) % 3]) == VerticesCount(all_a) + 2) {
                return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                 4 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a & Au[i])),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 1) % 3])),
                                 2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 2) % 3])),
                                 3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
            }
        }
        // 3.4.3.4.5
        return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                         2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[0] & a[1])),
                         2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[1] & a[2])),
                         2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[2] & a[0])),
                         3 + FindMaxIndependentSubGraph(graph.GetSubGraph(all_a))});
    }
    // 4
    if (degree == 4) {
        // 4.1
        if (degrees.back().first == 4) {
            // 4.1.1
            Vertex v = -1, w = -1;
            VertexSet Av, Aw;
            for (int i = 0; i < degrees.size() && degrees[i].first == 4 && v == -1; i++) {
                for (int j = degrees.size() - 1; j >= 0 && degrees[j].first != 4 && w == -1; j--) {
                    if (!graph.graph.HasEdge(degrees[i].second, degrees[j].second) &&
                        VerticesCount(GetVertexSetFromVector(graph.GetChildren(degrees[i].second)) &
                                      GetVertexSetFromVector(graph.GetChildren(degrees[j].second))) >= 2) {
                        v = degrees[i].second;
                        Av = graph.set & ~GetVertexId(v) & ~GetVertexSetFromVector(graph.GetChildren(v));
                        w = degrees[j].second;
                        Aw = graph.set & ~GetVertexId(w) & ~GetVertexSetFromVector(graph.GetChildren(w));
                    }
                }
            }
            if (v != -1) {
                // 4.1.1.1
                if (VerticesCount(GetVertexSetFromVector(graph.GetChildren(v)) &
                                  GetVertexSetFromVector(graph.GetChildren(w))) >= 3) {
                    return std::max(2 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw)),
                                    FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(v) & ~GetVertexId(w))));
                }
                // 4.1.1.2
                auto[x, y] = graph.GetSubGraph(GetVertexSetFromVector(graph.GetChildren(v)) &
                                               ~GetVertexSetFromVector(graph.GetChildren(w))).GetPair();
                VertexSet Ax = graph.set & ~GetVertexId(x) & ~GetVertexSetFromVector(graph.GetChildren(x));
                VertexSet Ay = graph.set & ~GetVertexId(y) & ~GetVertexSetFromVector(graph.GetChildren(y));
                auto[q, r] = graph.GetSubGraph(GetVertexSetFromVector(graph.GetChildren(w)) &
                                               ~GetVertexSetFromVector(graph.GetChildren(v))).GetPair();
                VertexSet Aq = graph.set & ~GetVertexId(q) & ~GetVertexSetFromVector(graph.GetChildren(q));
                VertexSet Ar = graph.set & ~GetVertexId(r) & ~GetVertexSetFromVector(graph.GetChildren(r));
                // 4.1.1.2.1
                if (graph.graph.HasEdge(x, y) && graph.graph.HasEdge(q, r)) {
                    return std::max(2 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw)),
                                    FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(v) & ~GetVertexId(w))));
                }
                // 4.1.1.2.2
                if (graph.graph.HasEdge(x, y) && !graph.graph.HasEdge(q, r)) {
                    return std::max({2 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw)),
                                     3 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw & Aq & Ar)),
                                     FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(v) & ~GetVertexId(w)))});
                }
                if (!graph.graph.HasEdge(x, y) && graph.graph.HasEdge(q, r)) {
                    return std::max({2 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw)),
                                     3 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw & Ax & Ay)),
                                     FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(v) & ~GetVertexId(w)))});
                }
                // 4.1.1.2.3
                if (VerticesCount(Av & Aw & Ax & Ay) >= graph.VerticesCount() - 9 ||
                    VerticesCount(Av & Aw & Aq & Ar) >= graph.VerticesCount() - 9) {
                    return std::max({3 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw & Aq & Ar)),
                                     3 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw & Ax & Ay)),
                                     FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(v) & ~GetVertexId(w)))});
                }
                // 4.1.1.2.4
                return std::max({2 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw)),
                                 3 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw & Aq & Ar)),
                                 3 + FindMaxIndependentSubGraph(graph.GetSubGraph(Av & Aw & Ax & Ay)),
                                 FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(v) & ~GetVertexId(w)))});
            }
            // 4.1.2
            std::vector<VertexSet> a(4);
            for (int i = 0; i < 4; i++) {
                a[i] = graph.set & ~GetVertexSetFromVector(graph.GetChildren(v)) &
                       ~GetVertexSetFromVector(graph.GetChildren(children[i]));
            }
            v = degrees.front().second;
            Av = graph.set & ~GetVertexId(v) & ~GetVertexSetFromVector(graph.GetChildren(v));
            // 4.1.2.1
            for (int i = 0; i < 4; i++) {
                if (graph.graph.HasEdge(children[i], children[(i + 1) % 4]) &&
                    graph.graph.HasEdge(children[i], children[(i + 2) % 4]) &&
                    graph.graph.HasEdge(children[i], children[(i + 3) % 4])) {
                    return 1;
                }
            }
            // 4.1.2.2
            for (int i = 0; i < 4; i++) {
                if (!graph.graph.HasEdge(children[i], children[(i + 1) % 4]) &&
                    !graph.graph.HasEdge(children[i], children[(i + 2) % 4]) &&
                    !graph.graph.HasEdge(children[i], children[(i + 3) % 4]) &&
                    graph.graph.HasEdge(children[(i + 1) % 4], children[(i + 2) % 4]) &&
                    graph.graph.HasEdge(children[(i + 2) % 4], children[(i + 3) % 4]) &&
                    graph.graph.HasEdge(children[(i + 3) % 4], children[(i + 1) % 4])) {
                    return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                     2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 1) % 4])),
                                     2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 2) % 4])),
                                     2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[i] & a[(i + 3) % 4]))});
                }
            }
            // 4.1.2.3
            std::vector<std::pair<int, int>> ids;
            for (int i = 0; i < 4; i++) {
                for (int j = i + 1; j < 4; j++) {
                    ids.emplace_back(i, j);
                }
            }
            for (int i = 0; i < 3; i++) {
                if (graph.graph.HasEdge(children[ids[i].first], children[ids[i].second]) &&
                    graph.graph.HasEdge(children[ids[5 - i].first], children[ids[5 - i].second]) &&
                    !graph.graph.HasEdge(children[ids[(i + 1) % 3].first], children[ids[(i + 1) % 3].second]) &&
                    !graph.graph.HasEdge(children[ids[5 - (i + 1) % 3].first],
                                         children[ids[5 - (i + 1) % 3].second]) &&
                    !graph.graph.HasEdge(children[ids[(i + 2) % 3].first], children[ids[(i + 2) % 3].second]) &&
                    !graph.graph.HasEdge(children[ids[5 - (i + 2) % 3].first],
                                         children[ids[5 - (i + 2) % 3].second])) {
                    return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].first] & a[ids[5 - i].first])),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].first] & a[ids[5 - i].second])),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].second] & a[ids[5 - i].first])),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].second] & a[ids[5 - i].second]))});
                }
            }
            // 4.1.2.4
            for (int i = 0; i < 6; i++) {
                if (graph.graph.HasEdge(children[ids[i].first], children[ids[i].second])) {
                    return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].first] & a[ids[5 - i].first])),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].first] & a[ids[5 - i].second])),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].second] & a[ids[5 - i].first])),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[i].second] & a[ids[5 - i].second])),
                                     2 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(a[ids[5 - i].first] & a[ids[5 - i].second])),
                                     3 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(
                                                     a[ids[i].first] & a[ids[5 - i].first] & a[ids[5 - i].second])),
                                     3 + FindMaxIndependentSubGraph(
                                             graph.GetSubGraph(
                                                     a[ids[i].second] & a[ids[5 - i].first] & a[ids[5 - i].second]))});
                }
            }
            // 4.1.2.5
            return std::max({1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                             2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[0] & a[1])),
                             2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[0] & a[2])),
                             2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[0] & a[3])),
                             2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[1] & a[2])),
                             2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[1] & a[3])),
                             2 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[2] & a[3])),
                             3 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[0] & a[1] & a[2])),
                             3 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[1] & a[2] & a[3])),
                             3 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[2] & a[3] & a[0])),
                             3 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[3] & a[0] & a[1])),
                             4 + FindMaxIndependentSubGraph(graph.GetSubGraph(a[0] & a[1] & a[2] & a[3]))});
        }
        // 4.2
        for (int i = 0; i < degrees.size() && degrees[i].first == 4; i++) {
            Vertex v = degrees[i].second;
            for (int j = degrees.size() - 1; j >= 0 && degrees[j].first != 4; j--) {
                Vertex w = degrees[j].second;
                if (graph.graph.HasEdge(degrees[i].second, degrees[j].first)) {
                    return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(w) &
                                                                                     ~GetVertexSetFromVector(
                                                                                             graph.GetChildren(w)))),
                                    FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(w))));
                }
            }
        }
    }
    // 5
    if (degrees.front().first == 5 && degrees.back().first == 5) {
        // 5.1
        if (graph.VerticesCount() == 6) {
            return 1;
        }
        // 5.2
        return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                        FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(vertex))));
    }
    // 6
    vertex = degrees.back().second;
    children = graph.GetChildren(vertex);
    set = ~GetVertexId(vertex) & ~GetVertexSetFromVector(children);
    return std::max(1 + FindMaxIndependentSubGraph(graph.GetSubGraph(set)),
                    FindMaxIndependentSubGraph(graph.GetSubGraph(~GetVertexId(vertex))));
}

int FindMaxIndependentSubGraph(const SubGraph &graph) {
    BOOST_LOG_TRIVIAL(info) << "Start finding max independent set";
    BOOST_LOG_TRIVIAL(info) << "Vertices mask : " << std::bitset<MAX_VERTICES_COUNT>(graph.set);
    auto connected_sets = graph.GetConnectedSubGraphs();
    BOOST_LOG_TRIVIAL(info) << "Connected subgraphs : " << connected_sets.size();
    int result = 0;
    int set_id = 1;
    for (const auto &connected : connected_sets) {
        BOOST_LOG_TRIVIAL(info) << "Finding in " << set_id << " subgraph";
        result += FindMaxIndependentSetInConnectedSubGraph(connected);
        set_id++;
    }
    BOOST_LOG_TRIVIAL(info) << "Independent set size: " << result << ", mask: "
                            << std::bitset<MAX_VERTICES_COUNT>(graph.set);
    return result;
}

void ReadGraph() {
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
}

int main() {
    ReadGraph();

    return 0;
}
