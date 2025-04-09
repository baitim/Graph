#pragma once

#include "Graph/common.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
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
        static constexpr size_t LENGTH_OF_OUTPUT_NUMBERS = 4;

        using edge_t = std::tuple<size_t, size_t, EdgeT>;
        using building_list_of_edges_t = std::unordered_map<size_t, std::vector<std::pair<size_t, EdgeT>>>;

        struct node_base_t {
            size_t index;

        public:
            node_base_t() = default;
            node_base_t(size_t index_) : index(index_) {}
            virtual std::ostream& print(std::ostream& os) const = 0;
            virtual ~node_base_t() = default;
        };

        class node_buffer_t final {
            std::vector<std::unique_ptr<node_base_t>> nodes_;

        public:
            template <typename NodeT, typename ...ArgsT>
            NodeT* add_node(ArgsT&&... args) {
                std::unique_ptr new_node = std::make_unique<NodeT>(std::forward<ArgsT>(args)...);
                nodes_.emplace_back(std::move(new_node));
                return static_cast<NodeT*>(nodes_.back().get());
            }
        };

    private:
        size_t count_verts_ = 0;
        size_t count_edges_ = 0;

        node_buffer_t buffer_nodes_;
        std::vector<node_base_t*> nodes_;
        std::vector<size_t> next_;
        std::vector<size_t> prev_;

    private:
        struct vertex_node_t final : public node_base_t {
            using node_base_t::index;
            VertexT data;

        public:
            vertex_node_t() = default;
            vertex_node_t(size_t index_) : node_base_t(index_) {}
            vertex_node_t(size_t index_, const VertexT& data_) : node_base_t(index_), data(data_) {}

            std::ostream& print(std::ostream& os) const override {
                os << print_lcyan('(' << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << index);
                if constexpr(!std::is_same_v<VertexT, std::monostate> &&
                             has_output_operator<VertexT>::value)
                    os << print_lcyan(", " << data);
                os << print_lcyan(')');
                return os;
            }
        };

        struct edge_node_t final : public node_base_t {
            using node_base_t::index;
            EdgeT data;

        public:
            edge_node_t() = default;
            edge_node_t(size_t index_) : node_base_t(index_) {}
            edge_node_t(size_t index_, const EdgeT& data_) : node_base_t(index_), data(data_) {}

            std::ostream& print(std::ostream& os) const override {
                os << print_lcyan('(' << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << index + 1);
                if constexpr(!std::is_same_v<EdgeT, std::monostate> &&
                             has_output_operator<EdgeT>::value)
                    os << print_lcyan(", " << data);
                os << print_lcyan(')');
                return os;
            }
        };

        template <bool IsConst>
        class internal_iterator_t final {
            template <typename ReferenceT>
            struct arrow_proxy final {
                ReferenceT reference;

            public:
                arrow_proxy(      ReferenceT& reference_) : reference(reference_) {}
                arrow_proxy(const ReferenceT& reference_) : reference(reference_) {}
                      ReferenceT *operator->()       { return &reference; }
                const ReferenceT *operator->() const { return &reference; }
            };

        private:
            using graph_type       = std::conditional_t<IsConst, const graph_t,        graph_t>;
            using vertex_reference = std::conditional_t<IsConst, const vertex_node_t&, vertex_node_t&>;
            using edge_reference   = std::conditional_t<IsConst, const edge_node_t&,   edge_node_t&>;

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type        = std::pair<vertex_node_t,  edge_node_t>;
            using reference         = std::pair<vertex_reference, edge_reference>;
            using pointer           = arrow_proxy<reference>;
            using difference_type   = std::ptrdiff_t;

            graph_type* graph_;
            size_t index_;

        private:
            std::tuple<size_t, size_t> get_current_position() const {
                size_t vertex = graph_->nodes_[index_]->index;
                size_t edge = index_ ^ 1;
                return {vertex, edge};
            }

        public:
            internal_iterator_t(graph_type& graph, size_t index)
            : graph_(&graph), index_(index) {}

            size_t index() const noexcept { return index_; }

            reference operator*() const {
                auto&& [vertex, edge] = get_current_position();
                return {*static_cast<vertex_node_t*>(graph_->nodes_[vertex]),
                        *static_cast<edge_node_t  *>(graph_->nodes_[edge])};
            }

                  pointer operator->()       noexcept { return **this; }
            const pointer operator->() const noexcept { return **this; }

            bool operator==(const internal_iterator_t& other) const noexcept {
                return ((graph_ == other.graph_) && (index_ == other.index_));
            }

            bool operator!=(const internal_iterator_t& other) const noexcept {
                return !(*this == other);
            }

            internal_iterator_t& operator++() noexcept {
                index_ = graph_->next_[index_];
                return *this;
            }

            internal_iterator_t& operator--() noexcept {
                index_ = graph_->prev_[index_];
                return *this;
            }
        };

        using       iterator_t = internal_iterator_t<false>;
        using const_iterator_t = internal_iterator_t<true>;

        template <bool IsConst>
        class internal_range_children_t final {
            using graph_type    = std::conditional_t<IsConst, const graph_t,    graph_t>;
            using iterator_type = std::conditional_t<IsConst, const_iterator_t, iterator_t>;

            graph_type* graph_;
            size_t start_index_;

        private:
            size_t get_end_index() const {
                if (start_index_ >= graph_->count_verts_)
                    return graph_->nodes_[start_index_]->index;
                return start_index_;
            }

        public:
            internal_range_children_t(graph_type& graph, size_t start_index)
            : graph_(&graph), start_index_(start_index) {}

            iterator_type begin() const { return ++iterator_type{*graph_, start_index_}; }
            iterator_type end()   const { return   iterator_type{*graph_, get_end_index()}; }
        };

        using       range_children_t = internal_range_children_t<false>;
        using const_range_children_t = internal_range_children_t<true>;

    private:
        static void check_vertex_indexes(int v1, int v2) {
            if (v1 <= 0 || v2 <= 0)
                throw error_t{"Invalid vertex index: <= 0"};
        }

        void check_vertex_index(size_t index) const {
            if (index < count_verts_)
                return;

            std::ostringstream oss;
            oss << "Invalid vertex index: "
                << "index: "       << index << ", "
                << "count_verts: " << count_verts_;
            throw error_t{oss.str()};
        }

        static bool read_edge(std::istream& is, edge_t& edge) {
            is >> std::ws;
            if (is.eof())
                return false;

            std::string dashes;
            char comma;
            int v1_, v2_;
            is >> v1_ >> dashes >> v2_;
            if (!is.good()) throw error_t{"Invalid edge indexes"};
            if (dashes != "--")
                throw error_t{"Invalid input: expected dashes between vertex indexes, read: " + dashes};
            check_vertex_indexes(v1_, v2_);

            EdgeT w_;
            if constexpr(!std::is_same_v<EdgeT, std::monostate> &&
                         has_input_operator<EdgeT>::value) {
                is >> comma >> w_;
                if (!is.good())   throw error_t{"Invalid edge info"};
                if (comma != ',') throw error_t{"Invalid input: expected comma, read: " + comma};
            }

            auto& [v1, v2, w] = edge;
            v1 = static_cast<size_t>(v1_ - 1);
            v2 = static_cast<size_t>(v2_ - 1);
            w = w_;
            return true;
        }

        void default_fill_vertices(size_t count) {
            for (size_t index = 0; index < count; ++index)
                nodes_[index] = buffer_nodes_.template add_node<vertex_node_t>(index);
        }

        void resize(size_t summary_count) {
            nodes_.resize(summary_count);
            next_.resize(summary_count);
            prev_.resize(summary_count);
            default_fill_vertices(count_verts_);
        }

        void create(const building_list_of_edges_t& edges) {
            count_verts_ += count_verts_ % 2;
            size_t summary_count = count_verts_ + 2 * count_edges_;
            resize(summary_count);

            std::vector<size_t> curr_idx(count_verts_);
            iota(curr_idx.begin(), curr_idx.end(), 0);
            
            size_t idx = count_verts_;
            for (auto&& edge : edges) {
                size_t current = edge.first;
                for (auto&& [child, edge_data] : edge.second) {
                    nodes_[idx]     = buffer_nodes_.template add_node<edge_node_t>(current, edge_data);
                    nodes_[idx + 1] = buffer_nodes_.template add_node<edge_node_t>(child,   edge_data);

                    next_[curr_idx[current]] = idx;
                    next_[curr_idx[child  ]] = idx + 1;

                    prev_[curr_idx[current]] = std::exchange(curr_idx[current], idx);
                    prev_[curr_idx[child  ]] = std::exchange(curr_idx[child  ], idx + 1);

                    idx += 2;
                }
            }

            for (auto i : std::views::iota(0UL, count_verts_)) {
                next_[curr_idx[i]] = i;
                prev_[i] = curr_idx[i];
            }
        }

        template <typename EdgeListT>
        void init_from_edges(const EdgeListT& edges_list) {
            building_list_of_edges_t edges;
            for (auto&& edge : edges_list) {
                if constexpr (std::tuple_size_v<std::decay_t<decltype(edge)>> == 2) {
                    auto [v1, v2] = edge;
                    check_vertex_indexes(v1--, v2--);
                    count_verts_ = std::max(count_verts_, 1 + std::max(v1, v2));
                    edges[v1].emplace_back(v2, EdgeT{});
                } else {
                    auto [v1, v2, w] = edge;
                    check_vertex_indexes(v1--, v2--);
                    count_verts_ = std::max(count_verts_, 1 + std::max(v1, v2));
                    edges[v1].emplace_back(v2, w);
                }
            }
            count_edges_ = edges_list.size();
            create(edges);
        }

    public:
        graph_t() {}

        graph_t(std::initializer_list<std::tuple<size_t, size_t>> edges) {
            init_from_edges(edges);
        }

        graph_t(std::initializer_list<std::tuple<size_t, size_t, const EdgeT&>> edges) {
            init_from_edges(edges);
        }

        void set_vertex_info(size_t index, const VertexT& info) {
            check_vertex_index(index);
            static_cast<vertex_node_t*>(nodes_[index])->data = info;
        }

        VertexT get_vertex_info(size_t index) {
            check_vertex_index(index);
            return static_cast<vertex_node_t*>(nodes_[index])->data;
        }

        std::istream& read(std::istream& is) {
            count_verts_ = 0;
            count_edges_ = 0;

            edge_t edge;
            building_list_of_edges_t edges;
            while (read_edge(is, edge)) {
                const auto& [v1, v2, w] = edge;
                count_verts_ = std::max(count_verts_, 1 + std::max(v1, v2));
                edges[v1].emplace_back(v2, w);
                count_edges_++;
            }
            create(edges);
            return is;
        }

        std::ostream& print(std::ostream& os) const {
            os << print_blue("graph\n");

            os << print_blue("nodes:\t");
            for (auto node : nodes_) {
                node->print(os);
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

        range_children_t get_range_children(size_t u) {
            return range_children_t{*this, u};
        }

        const_range_children_t get_range_children(size_t u) const {
            return const_range_children_t{*this, u};
        }

        size_t count_verts() const noexcept { return count_verts_; }
    };

    template <typename GraphT, typename Func, typename... Args>
    inline void do_dfs(const GraphT& graph, size_t start, Func func, Args&&... args) {
        start--;
        size_t count_verts = graph.count_verts();
        std::vector<bool> used(count_verts, false);
        std::vector<size_t> order;
        std::stack<size_t> s;
        order.reserve(count_verts);

        used[start] = true;
        s.push(start);
        while (!s.empty()) {
            size_t v = s.top();
            s.pop();
            order.push_back(v);

            for (auto i : graph.get_range_children(v)) {
                size_t next = i.second.index;
                if (!used[next]) {
                    used[next] = true;
                    s.push(next);
                }
            }
        }

        for (auto v : std::views::reverse(order))
            std::invoke(func, v, args...);
    }

    template <typename GraphT, typename Func, typename... Args>
    inline void do_bfs(const GraphT& graph, size_t start, Func func, Args&&... args) {
        start--;
        std::vector<bool> used(graph.count_verts(), false);
        std::vector<size_t> order;
        std::queue<size_t> q;

        used[start] = true;
        q.push(start);

        while (!q.empty()) {
            size_t v = q.front();
            q.pop();
            order.push_back(v);

            for (auto i : graph.get_range_children(v)) {
                size_t next = i.second.index;
                if (!used[next]) {
                    used[next] = true;
                    q.push(next);
                }
            }
        }

        for (auto v : order)
            std::invoke(func, v, args...);
    }

    inline std::vector<size_t> get_odd_cycle(size_t u, size_t v, size_t count_verts, std::span<int> parents) {
        if (u == v)
            return std::vector(3, u);

        std::vector<bool> visited(count_verts, false);
        std::vector<size_t> cycle;

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
        while (static_cast<int>(u) != lsa) {
            cycle.push_back(u + 1);
            u = parents[u];
        }

        return cycle;
    }

    struct get_bipartite_result_t final {
        bool is_bipartite;
        std::vector<int> colors;
        std::vector<size_t> cycle;
    };

    template <typename GraphT>
    inline get_bipartite_result_t get_bipartite(const GraphT& graph) {
        size_t count_verts = graph.count_verts();
        std::vector<int> colors (count_verts, -1);
        std::vector<int> parents(count_verts, -1);
        std::queue<size_t> q;

        for (auto v : std::views::iota(0UL, count_verts)) {
            if (colors[v] == -1) {
                q.push(v);
                colors[v] = 0;
    
                while (!q.empty()) {
                    size_t u = q.front();
                    q.pop();

                    for (auto i : graph.get_range_children(u)) {
                        size_t next = i.second.index;
                        if (colors[next] == -1) {
                            colors[next] = !colors[u];
                            parents[next] = u;
                            q.push(next);
                        } else if (colors[next] == colors[u]) {
                            return {false, {}, get_odd_cycle(u, next, count_verts, parents)};
                        }
                    }
                }
            }
        }
        return {true, colors, {}};
    }

    template <typename VertexT = std::monostate, typename EdgeT = std::monostate>
    inline std::istream& operator>>(std::istream& is, graph_t<VertexT, EdgeT>& graph) {
        return graph.read(is);
    }

    template <typename VertexT = std::monostate, typename EdgeT = std::monostate>
    inline std::ostream& operator<<(std::ostream& os, const graph_t<VertexT, EdgeT>& graph) {
        return graph.print(os);
    }
}