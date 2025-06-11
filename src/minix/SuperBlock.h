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

#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <cstdint>
#include <istream>

namespace minix {

constexpr unsigned int super_magic{0x4d5a};

struct SuperBlock {
    std::uint32_t ninodes;
    std::uint16_t pad0;
    std::uint16_t imap_blocks;
    std::uint16_t zmap_blocks;
    std::uint16_t first_data_zone;
    std::uint16_t log_zone_size;
    std::uint16_t pad1;
    std::uint32_t max_size;
    std::uint32_t nzones;
    std::uint16_t magic;
    std::uint16_t pad2;
    std::uint16_t block_size; // Block size in bytes
    std::uint8_t disk_version;
};

inline std::istream& operator>>(std::istream& is, SuperBlock& sb) {
    is.read(reinterpret_cast<char*>(&sb), sizeof(SuperBlock));
    if (sb.magic != super_magic) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

} // namespace minix

#endif // SUPERBLOCK_H
