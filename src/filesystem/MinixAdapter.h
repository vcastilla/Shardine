#ifndef MINIXADAPTER_H
#define MINIXADAPTER_H

#include <filesystem>
#include <string>
#include <vector>

#include "FileSystem.h"
#include "Segment.h"
#include "minix/MinixFS.h"

namespace fs {

class MinixAdapter final : public FileSystem {
public:
    explicit MinixAdapter(const std::filesystem::path& file_name);

private:
    const std::vector<Segment>& do_segments() const override {
        return m_segments;
    }

    bool do_write(const unsigned address, const char* data, const unsigned size) override {
        return m_fs.write(address, data, size);
    }

    bool do_flush() override {
        return m_fs.flush();
    }

    std::string do_name() override {
        return "MINIX 3";
    }

    [[nodiscard]] Structure boot_block() const;

    [[nodiscard]] Structure inode(unsigned index) const;

    [[nodiscard]] Structure superblock() const;

    [[nodiscard]] Structure inode_bitmap() const;

    [[nodiscard]] static constexpr unsigned min_inode_index() noexcept {
        return 1;
    }

    [[nodiscard]] constexpr unsigned max_inode_index() const noexcept {
        return m_fs.superblock().data.ninodes - 1;
    }

    [[nodiscard]] Structure data_bitmap() const;

    [[nodiscard]] Structure data_block(unsigned index) const;

    [[nodiscard]] constexpr unsigned min_data_index() const noexcept {
        return m_fs.superblock().data.first_data_zone;
    }

    [[nodiscard]] constexpr unsigned max_data_index() const noexcept {
        return m_fs.superblock().data.nzones - 1;
    }

    minix::MinixFS m_fs;
    std::vector<Segment> m_segments;
};


} // namespace fs

#endif // MINIXADAPTER_H
