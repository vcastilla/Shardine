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

#include "utils/BitMap.h"

#include <libassert/assert.hpp>

BitMap::BitMap(const size_type size) : m_data(size) {}

BitMap::BitMap(const size_type size, std::istream &is) : m_data(size) {
    is.read(reinterpret_cast<char *>(m_data.data()), size);
}

bool BitMap::operator[](const index_type index) const {
    ASSERT(index / 8 < m_data.size());
    return m_data[index / 8] & (1 << (index % 8));
}

std::uint8_t &BitMap::byte_at(const size_type offset) {
    ASSERT(offset < m_data.size());
    return m_data[offset];
}
