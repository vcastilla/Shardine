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

#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>
#include <iostream>
#include <vector>

class BitMap {
public:
    using size_type = unsigned int;
    using index_type = unsigned long;

    BitMap() = default;

    explicit BitMap(size_type size);

    BitMap(size_type size, std::istream &is);

    [[nodiscard]] constexpr const std::vector<std::uint8_t> &data() const noexcept {
        return m_data;
    }

    [[nodiscard]] size_type size() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] bool operator[](index_type index) const;

    std::uint8_t &byte_at(size_type offset);

private:
    std::vector<std::uint8_t> m_data;
};


#endif // BITMAP_H
