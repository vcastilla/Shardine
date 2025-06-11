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

#ifndef INODE_H
#define INODE_H

#include <cstdint>
#include <istream>

namespace minix {

struct INode {
    std::uint16_t mode;
    std::uint16_t nlinks;
    std::uint16_t uid;
    std::uint16_t gid;
    std::uint32_t size;
    std::uint32_t atime;
    std::uint32_t mtime;
    std::uint32_t ctime;
    std::uint32_t zone[7];
    std::uint32_t indirect_zone;
    std::uint32_t double_indirect_zone;
    std::uint32_t unused; // May be used for triple indirect zone
};

inline std::istream& operator>>(std::istream& is, INode& inode) {
    is.read(reinterpret_cast<char*>(&inode), sizeof(INode));
    return is;
}

} // namespace minix

#endif // INODE_H
