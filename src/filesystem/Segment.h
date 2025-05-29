#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstddef>
#include <functional>
#include <string>
#include "Structure.h"

namespace fs {

struct Segment {
    std::string name;
    std::size_t address{};
    std::size_t size{};
    std::function<Structure(std::size_t)> read;
    std::string element_name{name};
    std::size_t element_size{size};
    unsigned min_element_index{};
    unsigned max_element_index{};

    [[nodiscard]] constexpr bool is_single_element() const noexcept {
        return min_element_index == max_element_index;
    }
};

} // namespace fs

#endif // SEGMENT_H
