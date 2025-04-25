#pragma once

#include "Graph/ANSI_colors.hpp"

#include <stdexcept>
#include <variant>

namespace graph {
    class error_t : public std::runtime_error {
    public:
        error_t(std::string msg) : std::runtime_error(std::move(msg)) {}
    };

    template <typename T>
    concept not_monostate = !std::is_same_v<T, std::monostate>;

    template <typename T>
    concept has_input_operator = requires(std::istream& is, T& t) {
        { is >> t } -> std::same_as<std::istream&>;
    };

    template <typename T>
    concept has_output_operator = requires(std::ostream& os, const T& t) {
        { os << t } -> std::same_as<std::ostream&>;
    };
}