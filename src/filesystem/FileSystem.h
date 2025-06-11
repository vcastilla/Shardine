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

#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H

#include <cstddef>
#include <ranges>
#include <string>
#include <utility>
#include <vector>
#include "Segment.h"

namespace fs {

class FileSystem {
public:
    const std::vector<Segment>& segments() const {
        return do_segments();
    }

    bool write(unsigned address, const std::ranges::contiguous_range auto& byte_array) {
        const auto begin = std::ranges::data(byte_array);
        return do_write(address, reinterpret_cast<const char*>(begin), std::ranges::size(byte_array));
    }

    bool flush() {
        return do_flush();
    }

    std::string name() {
        return do_name();
    }

    virtual ~FileSystem() = default;

private:
    virtual const std::vector<Segment>& do_segments() const = 0;
    virtual bool do_write(unsigned address, const char* data, unsigned size) = 0;
    virtual bool do_flush() = 0;
    virtual std::string do_name() = 0;
};

inline std::vector<std::pair<Structure, Structure>> diff(const FileSystem& fs1, const FileSystem& fs2) {
    std::vector<std::pair<Structure, Structure>> result;

    const auto segments1 = fs1.segments();
    const auto segments2 = fs2.segments();

    auto seg1 = segments1.begin(), seg2 = segments2.begin();
    for (; seg1 != segments1.end(); ++seg1, ++seg2) {
        for (auto i = seg1->min_element_index; i <= seg1->max_element_index; ++i) {
            const auto structure1 = seg1->read(i);
            const auto structure2 = seg2->read(i);
            if (structure1.raw_data != structure2.raw_data)
                result.emplace_back(structure1, structure2);
        }
    }

    return result;
}

/* Returns the segment and index at the specified address. */
inline std::pair<Segment, unsigned> addr_to_segment(std::vector<Segment> segments, std::size_t addr) {
    auto segment = std::ranges::find_if(segments, [addr](const Segment& seg) { return seg.address > addr; });
    if (segment == std::ranges::begin(segments))
        return {};
    --segment;
    return {*segment, (addr - segment->address) / segment->element_size + segment->min_element_index};
}

} // namespace fs

#endif // IFILESYSTEM_H
