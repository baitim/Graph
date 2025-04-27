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
#include <vector>

namespace graph {
    template <typename VertexT = std::monostate, typename EdgeT = std::monostate>
    class graph_t final {
        using edge_t = std::tuple<size_t, size_t, EdgeT>;
        using building_list_of_edges_t = std::unordered_map<size_t, std::vector<std::pair<size_t, EdgeT>>>;

    private:
        size_t count_verts_ = 0;
        size_t count_edges_ = 0;

        std::vector<VertexT> v_data_;
        std::vector<EdgeT>   e_data_;
        std::vector<size_t>  edges_;
        std::vector<size_t>  next_;

    private:
        template <bool IsConstData>
        class iterator_data_t final {
            using vertex_value     = std::conditional_t<IsConstData, const VertexT,  VertexT>;
            using edge_value       = std::conditional_t<IsConstData, const EdgeT,    EdgeT>;
            using vertex_reference = vertex_value&;
            using edge_reference   = edge_value&;
            using vertex_pointer   = vertex_value*;
            using edge_pointer     = edge_value*;

            vertex_pointer vertex_;
            edge_pointer   edge_;
            size_t         index_;

        public:
            iterator_data_t(vertex_value& vertex, edge_value& edge, size_t index)
            : vertex_(&vertex), edge_(&edge), index_(index) {}

            vertex_reference vertex() const { return *vertex_; }
            edge_reference   edge()   const { return *edge_; }

            size_t index() const noexcept { return index_;}
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
            using graph_type        = std::conditional_t<IsConst, const graph_t,  graph_t>;
            using iterator_category = std::forward_iterator_tag;
            using value_type        = iterator_data_t<true>;
            using reference         = iterator_data_t<IsConst>;
            using pointer           = arrow_proxy<reference>;
            using difference_type   = std::ptrdiff_t;

            graph_type* graph_;
            size_t index_;
            size_t count_verts_;

        public:
            internal_iterator_t(graph_type& graph, size_t index)
            : graph_(&graph), index_(index), count_verts_(graph.count_verts()) {}

            size_t index() const noexcept { return index_; }

            reference operator*() const {
                size_t e_index = index_ - count_verts_;
                size_t vertex = graph_->edges_[e_index];
                size_t edge = e_index ^ 1;

                return {graph_->v_data_[vertex],
                        graph_->e_data_[edge / 2],
                        graph_->edges_[edge]};
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
        };

    public:
        using       iterator_t = internal_iterator_t<false>;
        using const_iterator_t = internal_iterator_t<true>;

    private:
        template <bool IsConst>
        class internal_range_children_t final {
            using graph_type    = std::conditional_t<IsConst, const graph_t,    graph_t>;
            using iterator_type = std::conditional_t<IsConst, const_iterator_t, iterator_t>;

            graph_type* graph_;
            size_t start_index_;

        public:
            internal_range_children_t(graph_type& graph, size_t start_index)
            : graph_(&graph), start_index_(start_index) {}

            iterator_type begin() const { return ++iterator_type{*graph_, start_index_}; }
            iterator_type end()   const { return   iterator_type{*graph_, start_index_}; }
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
            if constexpr(not_monostate<EdgeT> && has_input_operator<EdgeT>) {
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

        void resize(size_t count_verts, size_t count_edges) {
            size_t summary_count = count_verts + 2 * count_edges;

            v_data_.resize(count_verts);
            e_data_.resize(count_edges);
            edges_.resize(2 * count_edges);
            next_.resize(summary_count);
        }

        void create(const building_list_of_edges_t& edges) {
            count_verts_ += count_verts_ % 2;
            resize(count_verts_, count_edges_);

            std::vector<size_t> curr_idx(count_verts_);
            iota(curr_idx.begin(), curr_idx.end(), 0);
            
            size_t idx = 0;
            for (auto&& edge : edges) {
                size_t current = edge.first;
                for (auto&& [child, edge_data] : edge.second) {
                    edges_[idx]      = current;
                    edges_[idx + 1]  = child;
                    e_data_[idx / 2] = edge_data;

                    curr_idx[current]  =  next_[curr_idx[current]]  =  count_verts_ + idx;
                    curr_idx[child  ]  =  next_[curr_idx[child  ]]  =  count_verts_ + idx + 1;

                    idx += 2;
                }
            }

            for (auto i : std::views::iota(0UL, count_verts_))
                next_[curr_idx[i]] = i;
        }

        void add_edge(building_list_of_edges_t& edges, size_t v1, size_t v2, EdgeT data = {}) {
            check_vertex_indexes(v1--, v2--);
            count_verts_ = std::max(count_verts_, 1 + std::max(v1, v2));
            edges[v1].emplace_back(v2, data);
        }

        template <typename TupleT>
        void dispatch_edge_to_add(building_list_of_edges_t& edges, TupleT&& edge) {
            std::apply(
                [&](auto&&... args) {
                    add_edge(edges, std::forward<decltype(args)>(args)...);
                },
                edge
            );
        }

        template <typename EdgeListT>
        void init_from_edges(const EdgeListT& edges_list) {
            building_list_of_edges_t edges;
            for (auto&& edge : edges_list)
                dispatch_edge_to_add(edges, edge);
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

        void set_vertex_info(iterator_t iterator, const VertexT& info) {
            size_t index = iterator.index();
            check_vertex_index(index);
            v_data_[index] = info;
        }

        void set_vertex_info(iterator_t iterator, VertexT&& info) {
            size_t index = iterator.index();
            check_vertex_index(index);
            v_data_[index] = std::move(info);
        }

        const VertexT& get_vertex_info(const_iterator_t iterator) const {
            size_t index = iterator.index();
            check_vertex_index(index);
            return e_data_[index]->data;
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
            constexpr size_t LENGTH_OF_OUTPUT_NUMBERS = 4;

            os << print_blue("graph\n");

            os << print_blue("index:\t");
            for (auto index : std::ranges::iota_view(0UL, count_verts_ + 2 * count_edges_))
                os << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << print_lcyan(index) << '\t';
            os << '\n';

            os << print_blue("v_data:\t");
            for (auto v_data : v_data_) {
                if constexpr(not_monostate<VertexT> && has_output_operator<VertexT>)
                    os << print_lcyan(v_data);
                else
                    os << print_lcyan('-');
                os << '\t';
            }
            os << '\n';

            os << print_blue("e_data:\t");
            for (auto e_data : e_data_) {
                if constexpr(not_monostate<EdgeT> && has_output_operator<EdgeT>)
                    os << print_lcyan(e_data);
                else
                    os << print_lcyan('-');
                os << '\t';
            }
            os << '\n';

            os << print_blue("edges:\t");
            for (auto _ : std::ranges::iota_view(0UL, count_verts_))
                os << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << print_lcyan('-') << '\t';
            for (auto edge : edges_)
                os << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << print_lcyan(edge) << '\t';
            os << '\n';

            os << print_blue("next:\t");
            for (auto next : next_)
                os << std::setw(LENGTH_OF_OUTPUT_NUMBERS) << print_lcyan(next) << '\t';

            return os;
        }

        range_children_t get_range_children(iterator_t iterator) {
            return range_children_t{*this, iterator.index()};
        }

        const_range_children_t get_range_children(const_iterator_t iterator) const {
            return const_range_children_t{*this, iterator.index()};
        }

        size_t count_verts() const noexcept { return count_verts_; }
    };

    template <typename GraphT, typename Func, typename... Args>
    inline void do_dfs(const GraphT& graph, typename GraphT::const_iterator_t start,
                       Func&& func, Args&&... args) {
        std::stack<size_t> s;

        size_t count_verts = graph.count_verts();
        std::vector<bool> used(count_verts, false);
        std::vector<size_t> order;
        order.reserve(count_verts);

        size_t start_index = start.index();
        used[start_index] = true;
        s.push(start_index);
        while (!s.empty()) {
            size_t v = s.top();
            s.pop();
            order.push_back(v);

            for (auto i : graph.get_range_children({graph, v})) {
                size_t next = i.index();
                if (!used[next]) {
                    used[next] = true;
                    s.push(next);
                }
            }
        }

        for (auto v : std::views::reverse(order))
            std::invoke(std::forward<Func>(func), v, std::forward<Args>(args)...);
    }

    template <typename GraphT, typename Func, typename... Args>
    inline void do_bfs(const GraphT& graph, typename GraphT::const_iterator_t start,
                       Func&& func, Args&&... args) {
        std::queue<size_t> q;

        size_t count_verts = graph.count_verts();
        std::vector<bool> used(count_verts, false);
        std::vector<size_t> order;
        order.reserve(count_verts);

        size_t start_index = start.index();
        used[start_index] = true;
        q.push(start_index);
        while (!q.empty()) {
            size_t v = q.front();
            q.pop();
            order.push_back(v);

            for (auto i : graph.get_range_children({graph, v})) {
                size_t next = i.index();
                if (!used[next]) {
                    used[next] = true;
                    q.push(next);
                }
            }
        }

        for (auto v : order)
            std::invoke(std::forward<Func>(func), v, std::forward<Args>(args)...);
    }

    inline std::vector<size_t> get_odd_cycle(size_t u, size_t v, size_t count_verts,
                                             std::span<size_t> parents) {
        if (u == v)
            return std::vector(3, u);

        std::vector<bool> visited(count_verts, false);
        std::vector<size_t> cycle;

        size_t end_parent = count_verts + 1;

        for (size_t i = u; i != end_parent; i = parents[i])
            visited[i] = true;

        size_t lsa = end_parent;
        for (size_t i = v; i != end_parent; i = parents[i]) {
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

    struct get_bipartite_result_t final {
        bool is_bipartite;
        std::vector<int> colors;
        std::vector<size_t> cycle;
    };

    template <typename GraphT>
    inline get_bipartite_result_t get_bipartite(const GraphT& graph) {
        size_t count_verts = graph.count_verts();
        std::vector<int> colors (count_verts, -1);
        std::vector<size_t> parents(count_verts, count_verts + 1);
        std::queue<size_t> q;

        for (auto v : std::views::iota(0UL, count_verts)) {
            if (colors[v] == -1) {
                q.push(v);
                colors[v] = 0;
    
                while (!q.empty()) {
                    size_t u = q.front();
                    q.pop();

                    for (auto i : graph.get_range_children({graph, u})) {
                        size_t next = i.index();
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