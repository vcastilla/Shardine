#include "MinixFS.h"

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <libassert/assert.hpp>
#include <stdexcept>

#include "utils/utils.h"

namespace minix {

void MinixFS::init() {
    m_fstream.seekg(super_offset, std::ios::beg);
    m_fstream >> m_sb;
    if (!m_fstream) {
        throw std::runtime_error("File system does not have MINIX 3 magic number");
    }

    m_fstream.seekg(inode_bitmap_offset, std::ios::beg);
    const auto inode_bitmap_size = utils::to_unsigned(m_sb.imap_blocks * m_sb.block_size);
    m_inode_bitmap = {inode_bitmap_size, m_fstream};

    m_data_bitmap_offset = inode_bitmap_offset + inode_bitmap_size;
    m_fstream.seekg(m_data_bitmap_offset, std::ios::beg);
    const auto data_bitmap_size = utils::to_unsigned(m_sb.zmap_blocks * m_sb.block_size);
    m_data_bitmap = {data_bitmap_size, m_fstream};

    m_root_ino_offset = inode_bitmap_offset + inode_bitmap_size + data_bitmap_size;
}

MinixFS::MinixFS(const std::filesystem::path& file_name) : m_fstream{file_name} {
    if (!m_fstream) {
        throw std::runtime_error(std::strerror(errno));
    }
    init();
}

address<std::vector<std::byte>> MinixFS::boot_block() const noexcept {
    std::vector<std::byte> data(super_offset);
    m_fstream.seekg(boot_offset, std::ios::beg);
    m_fstream.read(reinterpret_cast<char*>(data.data()), super_offset);
    return {boot_offset, data};
}

address<INode> MinixFS::inode(const unsigned index) const {
    ASSERT(index != 0);
    ASSERT(index < m_sb.ninodes);
    const auto offset = m_root_ino_offset + (index - 1) * static_cast<unsigned>(sizeof(INode));
    return {offset, utils::stream_init<INode>(m_fstream, offset)};
}

address<std::vector<std::byte>> MinixFS::block(const unsigned index) const {
    ASSERT(index < m_sb.nzones);
    const auto offset = m_sb.block_size * index;
    std::vector<std::byte> data(m_sb.block_size);
    m_fstream.seekg(offset, std::ios::beg);
    m_fstream.read(reinterpret_cast<char*>(data.data()), m_sb.block_size);
    return {offset, data};
}

} // namespace minix
