/*
 * Copyright 2025 Víctor Castilla Rodríguez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstddef>
#include <functional>
#include <string>
#include "Structure.h"

namespace fs {

struct Segment {
    QString name;
    std::size_t address{};
    std::size_t size{};
    std::function<Structure(std::size_t)> read;
    QString element_name{name};
    std::size_t element_size{size};
    unsigned min_element_index{};
    unsigned max_element_index{};

    [[nodiscard]] constexpr bool is_single_element() const noexcept {
        return min_element_index == max_element_index;
    }
};

} // namespace fs

#endif // SEGMENT_H
