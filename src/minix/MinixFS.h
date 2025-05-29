#ifndef MINIXFS_H
#define MINIXFS_H

#include <bitset>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <vector>

#include "INode.h"
#include "SuperBlock.h"
#include "utils/BitMap.h"
#include "utils/utils.h"

namespace minix {

constexpr unsigned int boot_offset{0};
constexpr unsigned int super_offset{1024};
constexpr unsigned int inode_bitmap_offset{2048};

template<typename Data>
struct address {
    std::size_t offset;
    Data data;
};

struct Info {
    std::size_t begin_address{};
    std::size_t size{};
    std::size_t end_address{begin_address + size};
};

class MinixFS {
public:
    explicit MinixFS(const std::filesystem::path& file_name);

    [[nodiscard]] address<std::vector<std::byte>> boot_block() const noexcept;

    [[nodiscard]] static constexpr Info boot_block_info() noexcept {
        return {boot_offset, super_offset};
    }

    [[nodiscard]] constexpr address<const SuperBlock&> superblock() const noexcept {
        return {super_offset, m_sb};
    }

    [[nodiscard]] static constexpr Info superblock_info() noexcept {
        return {super_offset, sizeof(SuperBlock), inode_bitmap_offset};
    }

    [[nodiscard]] constexpr address<const BitMap&> inode_bitmap() const noexcept {
        return {inode_bitmap_offset, m_inode_bitmap};
    }

    [[nodiscard]] Info inode_bitmap_info() const noexcept {
        return {inode_bitmap_offset, m_inode_bitmap.size()};
    }

    [[nodiscard]] constexpr address<const BitMap&> data_bitmap() const noexcept {
        return {m_data_bitmap_offset, m_data_bitmap};
    }

    [[nodiscard]] Info data_bitmap_info() const noexcept {
        return {m_data_bitmap_offset, m_data_bitmap.size()};
    }

    [[nodiscard]] address<INode> inode(unsigned index) const;

    [[nodiscard]] constexpr Info inode_info() const {
        return {m_root_ino_offset, m_sb.ninodes * sizeof(INode)};
    }

    [[nodiscard]] address<std::vector<std::byte>> block(unsigned index) const;

    [[nodiscard]] constexpr Info block_info() const {
        const auto begin = utils::cast<std::size_t>(m_sb.first_data_zone * m_sb.block_size);
        const auto end = utils::cast<std::size_t>(m_sb.nzones * m_sb.block_size);
        return {begin, end - begin, end};
    }

    bool write(const unsigned address, const std::ranges::contiguous_range auto& data) {
        m_fstream.seekg(address, std::ios::beg);
        const auto begin = std::to_address(std::ranges::begin(data));
        m_fstream.write(reinterpret_cast<const char*>(begin), std::ranges::size(data));
        return m_fstream.good();
    }

    bool write(const unsigned address, const char* data, const unsigned size) {
        m_fstream.seekg(address, std::ios::beg);
        m_fstream.write(data, size);
        return m_fstream.good();
    }

    bool flush() {
        m_fstream.flush();
        return m_fstream.good();
    }

private:
    void init();

    // Mutable stream to allow for const read functions
    mutable std::fstream m_fstream;

    SuperBlock m_sb{};
    BitMap m_inode_bitmap;
    BitMap m_data_bitmap;

    unsigned m_data_bitmap_offset{};
    unsigned m_root_ino_offset{};
};

} // namespace minix

#endif // MINIXFS_H
