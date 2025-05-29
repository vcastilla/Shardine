#include "MinixAdapter.h"

#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#include "Field.h"
#include "utils/strmode.h"
#include "utils/utils.h"

namespace fs {

MinixAdapter::MinixAdapter(const std::filesystem::path& file_name) : m_fs{file_name} {
    constexpr auto boot_info = minix::MinixFS::boot_block_info();
    constexpr auto sb_info = minix::MinixFS::superblock_info();
    const auto ino_bitmap_info = m_fs.inode_bitmap_info();
    const auto data_bitmap_info = m_fs.data_bitmap_info();
    const auto ino_info = m_fs.inode_info();
    const auto data_block_info = m_fs.block_info();

    m_segments = {
            {"Boot block", boot_info.begin_address, boot_info.size, [this](auto) { return boot_block(); }},
            {"Superblock", sb_info.begin_address, sb_info.end_address - sb_info.begin_address,
             [this](auto) { return superblock(); }},
            {"Inode bitmap", ino_bitmap_info.begin_address, ino_bitmap_info.size,
             [this](auto) { return inode_bitmap(); }},
            {"Data bitmap", data_bitmap_info.begin_address, data_bitmap_info.size,
             [this](auto) { return data_bitmap(); }},
            {"Inodes", ino_info.begin_address, ino_info.size, [this](auto idx) { return inode(idx); }, "Inode",
             sizeof(minix::INode), min_inode_index(), max_inode_index()},
            {"Data blocks", data_block_info.begin_address, data_block_info.size,
             [this](auto idx) { return data_block(idx); }, "Data block", m_fs.superblock().data.block_size,
             min_data_index(), max_data_index()},
    };
}

constexpr auto empty_str = [](std::uint32_t) { return ""; };

Structure MinixAdapter::boot_block() const {
    const auto [offset, bblock] = m_fs.boot_block();
    const QByteArray block_bytes{reinterpret_cast<const char*>(bblock.data()), utils::to_signed(bblock.size())};
    return {offset, block_bytes, {}};
}

Structure MinixAdapter::inode(const unsigned index) const {
    ASSERT(index >= min_inode_index());
    ASSERT(index <= max_inode_index());

    const auto [offset, ino] = m_fs.inode(index);

    const QByteArray ino_bytes{reinterpret_cast<const char*>(&ino), sizeof(ino)};

    const std::vector<Field> fields{
            {"File type and rwx bits", ino.mode, ino::mode_to_str},
            {"No. references", ino.nlinks},
            {"Owner ID", ino.uid},
            {"Group ID", ino.gid},
            {"File size in bytes", ino.size},
            {"Last accessed", ino.atime, utils::timestamp_to_str},
            {"Modified", ino.mtime, utils::timestamp_to_str},
            {"Status changed", ino.ctime, utils::timestamp_to_str},
            {"Direct zone 0", ino.zone[0]},
            {"Direct zone 1", ino.zone[1]},
            {"Direct zone 2", ino.zone[2]},
            {"Direct zone 3", ino.zone[3]},
            {"Direct zone 4", ino.zone[4]},
            {"Direct zone 5", ino.zone[5]},
            {"Direct zone 6", ino.zone[6]},
            {"Single indirect zone", ino.indirect_zone},
            {"Double indirect zone", ino.double_indirect_zone},
            {"(Unused)", ino.unused, empty_str},
    };

    return {offset, ino_bytes, fields};
}

Structure MinixAdapter::superblock() const {
    const auto [offset, sb] = m_fs.superblock();

    const QByteArray sb_bytes{reinterpret_cast<const char*>(&sb), sizeof(sb)};

    const std::vector<Field> fields{
            {"Maximum no. of i-nodes", sb.ninodes},
            {"(Padding)", sb.pad0, empty_str},
            {"No. of i-node bitmap blocks", sb.imap_blocks},
            {"No. of data bitmap blocks", sb.zmap_blocks},
            {"First data zone", sb.first_data_zone},
            {"log2(block/zone)", sb.log_zone_size},
            {"(Padding)", sb.pad1, empty_str},
            {"Maximum file size in bytes", sb.max_size},
            {"No. total zones", sb.nzones},
            {"Magic number", sb.magic},
            {"(Padding)", sb.pad2, empty_str},
            {"Block size in bytes", sb.block_size},
            {"File system sub-version", sb.disk_version, [](auto x) { return std::to_string(static_cast<int>(x)); }},
    };

    return {offset, sb_bytes, fields};
}

Structure MinixAdapter::inode_bitmap() const {
    const auto [offset, bitmap] = m_fs.inode_bitmap();
    const QByteArray bitmap_bytes{reinterpret_cast<const char*>(bitmap.data().data()), bitmap.size()};
    return {offset, bitmap_bytes, {}};
}

Structure MinixAdapter::data_bitmap() const {
    const auto [offset, bitmap] = m_fs.data_bitmap();
    const QByteArray bitmap_bytes{reinterpret_cast<const char*>(bitmap.data().data()), bitmap.size()};
    return {offset, bitmap_bytes, {}};
}

Structure MinixAdapter::data_block(const unsigned index) const {
    const auto [offset, dblock] = m_fs.block(index);
    const QByteArray block_bytes{reinterpret_cast<const char*>(dblock.data()), utils::to_signed(dblock.size())};
    return {offset, block_bytes, {}};
}


} // namespace fs
