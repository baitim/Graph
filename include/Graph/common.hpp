#pragma once

#include "Graph/ANSI_colors.hpp"
#include <stdexcept>

namespace graph {
    class error_t : public std::runtime_error {
    public:
        error_t(std::string msg) : std::runtime_error(std::move(msg)) {}
    };

    template <typename T, typename = void>
    struct has_input_operator : std::false_type {};

    template <typename T>
    struct has_input_operator<T, std::void_t<decltype(std::declval<std::istream&>() >> std::declval<T&>())>> 
        : std::true_type {};

    template <typename T, typename = void>
    struct has_output_operator : std::false_type {};
    
    template <typename T>
    struct has_output_operator<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> 
        : std::true_type {};
}