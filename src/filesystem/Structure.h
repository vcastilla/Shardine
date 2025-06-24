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

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <QByteArray>
#include <cstddef>
#include <functional>
#include <libassert/assert.hpp>
#include <string>
#include <vector>
#include "Field.h"

namespace fs {

using ByteArray = QByteArray;

struct Structure {
    std::size_t address{};
    ByteArray raw_data;
    std::vector<Field> fields;

    [[nodiscard]] std::size_t idx_to_size(const std::size_t idx) const {
        ASSERT(idx < fields.size());
        std::size_t cnt = 0;
        for (std::size_t i = 0; i < idx; i++)
            cnt += fields[i].byte_size();
        return cnt;
    }

    [[nodiscard]] std::size_t size_to_idx(const std::size_t size) const {
        std::size_t current_idx = 0;
        std::size_t current_offset = 0;
        for (const auto& field: fields) {
            current_offset += field.byte_size();
            if (current_offset > size)
                break;
            current_idx++;
        }
        return current_idx;
    }

    [[nodiscard]] QString name_at(const std::size_t idx) const {
        ASSERT(idx < fields.size());
        return fields[idx].name();
    }

    [[nodiscard]] std::string value_at(const std::size_t idx, const std::size_t offset = 0) const {
        const auto size = idx_to_size(idx);
        const auto off = ASSERT_VAL(fields[idx].array_elem_size() * offset < fields[idx].byte_size());
        ASSERT(size + off < raw_data.size());
        return fields[idx].value(&raw_data.data()[size + off]);
    }

    [[nodiscard]] std::size_t field_size_at(const std::size_t idx) const {
        ASSERT(idx < fields.size());
        return fields[idx].byte_size();
    }

    friend bool operator==(const Structure& lhs, const Structure& rhs) {
        return lhs.address == rhs.address && lhs.raw_data == rhs.raw_data;
    }
};

} // namespace fs

#endif // STRUCTURE_H
