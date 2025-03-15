#pragma once

#include "Graph/common.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <ranges>
#include <stack>
#include <utility>
#include <variant>
#include <vector>

namespace graph {
    template <typename VertexT = std::monostate, typename EdgeT = std::monostate>
    class graph_t final {
        static const size_t LENGTH_OF_OUTPUT_NUMBERS = 4;

        size_t count_verts_;
        size_t count_edges_;
        bool   is_verts_odd_;

        struct vertex_node_t;
        struct edge_node_t;
        std::vector<vertex_node_t> vertexes_;
        std::vector<edge_node_t> edges_;
        std::vector<size_t> next_;
        std::vector<size_t> prev_;

        using edge_t = std::tuple<size_t, size_t, EdgeT>;

    private:
        struct vertex_node_t final {
            size_t index;
            VertexT data;

        public:
            vertex_node_t& operator=(size_t new_index) {
                index = new_index;
                return *this;
            }

            std::ostream& print(std::ostream& os) const {
                os << print_lcyan('(' << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << index);
                if constexpr(!std::is_same_v<VertexT, std::monostate> &&
                             has_output_operator<VertexT>::value)
                    os << print_lcyan(", " << data);
                os << print_lcyan(')');
                return os;
            }
        };

        struct edge_node_t final {
            size_t index;
            EdgeT data;

        public:
            edge_node_t() = default;
            edge_node_t(size_t index_, const EdgeT& data_) : index(index_), data(data_) {}

            std::ostream& print(std::ostream& os) const {
                os << print_lcyan('(' << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << index + 1);
                if constexpr(!std::is_same_v<EdgeT, std::monostate> &&
                             has_output_operator<EdgeT>::value)
                    os << print_lcyan(", " << data);
                os << print_lcyan(')');
                return os;
            }
        };

        template <typename GraphT>
        class internal_iterator_t final {
            using vertex_node_t = GraphT::vertex_node_t;
            using edge_node_t   = GraphT::edge_node_t;

            template <typename ReferenceT>
            struct arrow_proxy final {
                ReferenceT reference;
            
            public:
                arrow_proxy(ReferenceT& reference_) : reference(reference_) {}
                arrow_proxy(const ReferenceT& reference_) : reference(reference_) {}
                ReferenceT *operator->() { return &reference; }
                const ReferenceT *operator->() const { return &reference; }
            };

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = std::pair<vertex_node_t,  edge_node_t>;
            using reference = std::pair<
                std::conditional_t<std::is_const_v<GraphT>, const vertex_node_t&, vertex_node_t&>,
                std::conditional_t<std::is_const_v<GraphT>, const edge_node_t&,   edge_node_t&>
            >;
            using pointer           = arrow_proxy<reference>;
            using difference_type   = std::ptrdiff_t;

            GraphT& graph_;
            size_t index_;

        public:
            internal_iterator_t(GraphT& graph, size_t index)
            : graph_(graph), index_(index) {}

            size_t index() const noexcept { return index_; }

            reference operator*() {
                size_t t_index = index_ - graph_.count_verts_;
                size_t vertex = graph_.edges_[t_index].index;
                size_t edge = t_index ^ 1;
                return {graph_.vertexes_[vertex], graph_.edges_[edge]};
            }

            const reference operator*() const {
                size_t t_index = index_ - graph_.count_verts_;
                size_t vertex = graph_.edges_[t_index].index;
                size_t edge = t_index ^ 1;
                return {graph_.vertexes_[vertex], graph_.edges_[edge]};
            }

            pointer operator->()             noexcept { return **this; }
            const pointer operator->() const noexcept { return **this; }

            bool operator==(const internal_iterator_t& rhs) const noexcept {
                return (index_ == rhs.index_);
            }

            bool operator!=(const internal_iterator_t& rhs) const noexcept {
                return !(index_ == rhs.index_);
            }

            internal_iterator_t& operator++() noexcept {
                index_ = graph_.next_[index_];
                return *this;
            }

            internal_iterator_t& operator--() noexcept {
                index_ = graph_.prev_[index_];
                return *this;
            }
        };

        template <typename GraphT>
        class range_traversal_neighbors final {
            GraphT& graph_;
            internal_iterator_t<GraphT> iterator_;

        public:
            template <typename... ArgsT>
            range_traversal_neighbors(GraphT& graph, ArgsT&&... args)
            : graph_(graph), iterator_(graph, std::forward<ArgsT>(args)...) {
                ++iterator_;
            }

            internal_iterator_t<GraphT> begin() const {
                return iterator_;
            }

            internal_iterator_t<GraphT> end() const {
                size_t index = iterator_.index();
                if (index >= graph_.count_verts_)
                    index = iterator_->first.index;
                return {graph_, index};
            }
        };

    private:
        static std::istream& read_edge(std::istream& is, edge_t& edge) {
            char dummy;
            int v1_, v2_;
            EdgeT w_;
            is >> v1_ >> std::ws >> dummy >> dummy >> std::ws >> v2_;
            
            if constexpr(!std::is_same_v<EdgeT, std::monostate> &&
                         has_input_operator<EdgeT>::value)
                is >> dummy >> std::ws >> w_;

            if (!is.good() && !is.eof())
                throw error_t{str_red("Invalid edge")};
            if (v1_ <= 0 && !is.eof())
                throw error_t{str_red("Vertex(1, src) index <= 0")};
            if (v2_ <= 0 && !is.eof())
                throw error_t{str_red("Vertex(2, dst) index <= 0")};

            auto& [v1, v2, w] = edge;
            v1 = static_cast<size_t>(v1_ - 1);
            v2 = static_cast<size_t>(v2_ - 1);
            w = w_;
            return is;
        }

        void create(const std::vector<std::vector<std::pair<size_t, EdgeT>>>& edges) {
            size_t summary_count = count_verts_ + 2 * count_edges_;

            vertexes_.resize(count_verts_);
            edges_.resize(2 * count_edges_);
            next_.resize(summary_count);
            prev_.resize(summary_count);

            iota(vertexes_.begin(), vertexes_.end(), 0);

            size_t idx = count_verts_;
            std::vector<size_t> curr_idx(count_verts_);
            iota(curr_idx.begin(), curr_idx.end(), 0);

            for (auto i : std::views::iota(0UL, count_verts_)) {
                for (auto edge : edges[i]) {
                    size_t v1 = i;
                    size_t v2 = edge.first;
                    EdgeT edge_data = edge.second;

                    edges_[idx - count_verts_]     = edge_node_t{v1, edge_data};
                    edges_[idx - count_verts_ + 1] = edge_node_t{v2, edge_data};

                    next_[curr_idx[v1]] = idx;
                    next_[curr_idx[v2]] = idx + 1;

                    prev_[curr_idx[v1]] = std::exchange(curr_idx[v1], idx);
                    prev_[curr_idx[v2]] = std::exchange(curr_idx[v2], idx + 1);

                    idx += 2;
                }
            }

            for (auto i : std::views::iota(0UL, count_verts_)) {
                next_[curr_idx[i]] = i;
                prev_[i] = curr_idx[i];
            }
        }

        std::vector<int> get_odd_cycle(int u, int v, const std::vector<int>& parents) const {
            std::vector<bool> visited(count_verts_, false);
            std::vector<int> cycle;

            for (int i = u; i != -1; i = parents[i])
                visited[i] = true;

            int lsa = -1;
            for (int i = v; i != -1; i = parents[i]) {
                cycle.push_back(i + 1);
                if (visited[i]) {
                    lsa = i;
                    break;
                }
            }

            std::reverse(cycle.begin(), cycle.end());
            while (u != lsa) {
                cycle.push_back(u + 1);
                u = parents[u];
            }

            return cycle;
        }

    public:
        graph_t() {}

        graph_t(std::initializer_list<std::tuple<size_t, size_t>> edges_) {
            count_edges_ = 0;
            count_verts_ = 0;
            std::vector<std::vector<std::pair<size_t, EdgeT>>> edges;
            for (auto edge : edges_) {
                auto [v1, v2] = edge;
                v1--;
                v2--;

                count_verts_ = std::max(count_verts_, 1 + std::max(v1, v2));
                is_verts_odd_ = count_verts_ % 2;
                count_verts_ += is_verts_odd_;

                if (count_verts_ > edges.size())
                    edges.resize(count_verts_);

                edges[v1].push_back({v2, {}});

                count_edges_++;
            }
            
            create(edges);
        }

        std::istream& read(std::istream& is) {
            count_edges_ = 0;
            count_verts_ = 0;
            edge_t edge;
            std::vector<std::vector<std::pair<size_t, EdgeT>>> edges;
            while (read_edge(is, edge)) {
                const auto& [v1, v2, w] = edge;

                count_verts_ = std::max(count_verts_, 1 + std::max(v1, v2));
                is_verts_odd_ = count_verts_ % 2;
                count_verts_ += is_verts_odd_;

                if (count_verts_ > edges.size())
                    edges.resize(count_verts_);

                edges[v1].push_back({v2, w});

                count_edges_++;
            }
            
            create(edges);
            return is;
        }

        std::ostream& print(std::ostream& os) const {
            os << print_blue("graph\n");

            os << print_blue("vertexes:\t");
            for (auto vertex : vertexes_) {
                vertex.print(os);
                os << '\t';
            }
            os << '\n';

            os << print_blue("edges:\t");
            for (auto edge : edges_) {
                edge.print(os);
                os << '\t';
            }
            os << '\n';

            os << print_blue("n:\t");
            for (auto next : next_)
                os << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << print_lcyan(next) << '\t';
            os << '\n';

            os << print_blue("p:\t");
            for (auto prev : prev_)
                os << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << print_lcyan(prev) << '\t';

            return os;
        }

        template <typename Func, typename... Args>
        void dfs(size_t start, Func func, Args&&... args) const {
            start--;
            std::vector<bool> used(count_verts_, false);
            std::vector<size_t> order;
            std::stack<size_t> s;
            order.reserve(count_verts_);

            used[start] = true;
            s.push(start);
            while (!s.empty()) {
                size_t v = s.top();
                s.pop();
                order.push_back(v);

                for (auto i : range_traversal_neighbors(*this, v)) {
                    size_t next = i.second.index;
                    if (!used[next]) {
                        used[next] = true;
                        s.push(next);
                    }
                }
            }

            for (auto v : std::views::reverse(order))
                func(v, std::forward<Args>(args)...);
        }

        template <typename Func, typename... Args>
        void bfs(size_t start, Func func, Args&&... args) const {
            start--;
            
            std::vector<bool> used(count_verts_, false);
            std::vector<size_t> order;
            std::queue<size_t> q;

            used[start] = true;
            q.push(start);

            while (!q.empty()) {
                size_t v = q.front();
                q.pop();
                order.push_back(v);

                for (auto i : range_traversal_neighbors(*this, v)) {
                    size_t next = i.second.index;
                    if (!used[next]) {
                        used[next] = true;
                        q.push(next);
                    }
                }
            }

            for (auto v : order)
                func(v, std::forward<Args>(args)...);
        }

        std::tuple<bool, std::vector<int>, std::vector<int>> get_bipartite() const {
            std::vector<int> colors (count_verts_, -1);
            std::vector<int> parents(count_verts_, -1);
            std::queue<size_t> q;
        
            for (auto v : std::views::iota(0UL, count_verts_)) {
                if (colors[v] == -1) {
                    q.push(v);
                    colors[v] = 0;
        
                    while (!q.empty()) {
                        size_t u = q.front();
                        q.pop();

                        for (auto i : range_traversal_neighbors(*this, u)) {
                            size_t next = i.second.index;
                            if (colors[next] == -1) {
                                colors[next] = !colors[u];
                                parents[next] = u;
                                q.push(next);
                            } else if (colors[next] == colors[u]) {
                                return {false, {}, get_odd_cycle(u, next, parents)};
                            }
                        }
                    }
                }
            }
        
            return {true, colors, {}};
        }
    };

    template <typename VertexT = std::monostate, typename EdgeT = std::monostate>
    inline std::istream& operator>>(std::istream& is, graph_t<VertexT, EdgeT>& graph) {
        return graph.read(is);
    }

    template <typename VertexT = std::monostate, typename EdgeT = std::monostate>
    inline std::ostream& operator<<(std::ostream& os, const graph_t<VertexT, EdgeT>& graph) {
        return graph.print(os);
    }
}