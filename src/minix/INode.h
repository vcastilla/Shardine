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
