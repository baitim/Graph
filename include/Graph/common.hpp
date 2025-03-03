#pragma once

#include "Graph/ANSI_colors.hpp"
#include <stdexcept>

namespace graph {
    template <typename MsgT>
    concept error_str =
    std::is_constructible_v<std::string, MsgT> &&
    requires(std::ostream& os, MsgT msg) {
        { os << msg } -> std::same_as<std::ostream&>;
    };

    class error_t : public std::runtime_error {
    public:
        template <error_str MsgT>
        error_t(MsgT msg) : std::runtime_error(msg) {}
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