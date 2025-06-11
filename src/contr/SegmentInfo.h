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

#ifndef SEGMENTINFO_H
#define SEGMENTINFO_H

#include <QString>
#include <cstddef>

struct SegmentInfo {
    QString name;
    QString alt_text;
    QString elem_name;

    int min_elem_idx;
    int max_elem_idx;
    std::size_t size;

    [[nodiscard]] constexpr bool is_single_element() const noexcept {
        return min_elem_idx == max_elem_idx;
    }
};

#endif // SEGMENTINFO_H
