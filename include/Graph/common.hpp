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
}