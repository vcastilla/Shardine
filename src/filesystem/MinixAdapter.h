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

#ifndef MINIXADAPTER_H
#define MINIXADAPTER_H

#include <QCoreApplication>
#include <filesystem>
#include <string>
#include <vector>

#include "FileSystem.h"
#include "Segment.h"
#include "minix/MinixFS.h"

namespace fs {

class MinixAdapter final : public FileSystem {
    Q_DECLARE_TR_FUNCTIONS(MinixAdapter)

public:
    explicit MinixAdapter(const std::filesystem::path& file_name);

    static constexpr auto fs_name = "MINIX 3";

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
