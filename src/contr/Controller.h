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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QFileSystemWatcher>
#include <QString>
#include <QTemporaryFile>
#include <memory>
#include <tl/expected.hpp>
#include <vector>

#include "OperationStack.h"
#include "SegmentInfo.h"
#include "filesystem/FileSystem.h"
#include "qt-utils/MountableDevice.h"

class Controller final : public QObject {
    Q_OBJECT
public:
    // File management
    bool open(const std::filesystem::path& file_name);
    bool save();
    bool save_as(const std::filesystem::path& file_name);
    bool save_history(const std::filesystem::path& file_name);
    bool close();

    // File system
    QString fs_name() const;
    const std::vector<SegmentInfo>& get_segments();
    fs::Structure get_structure(unsigned segment_idx, unsigned elem_idx);
    tl::expected<QString, QString> fsck();

    // Mount
    bool mount(const QString& str);
    bool is_mounted() const;
    bool umount();

    // Change history
    void connect_undo_view(QUndoView& view);
    void add_change(const fs::Structure& structure);
    void reload_changes();
    bool is_clean() const;
    bool are_new_changes_unsaved() const;

signals:
    // Mounted device
    void mount_status_changed(bool mounted);
    void mounted();
    void unmounted();

    // File changes
    void file_changed();

private:
    void generate_segments();
    void backup();

    std::unique_ptr<fs::FileSystem> m_file_system;
    std::unique_ptr<fs::FileSystem> m_fs_backup;

    std::unique_ptr<MountableDevice> m_device;
    QFileSystemWatcher m_watcher;

    OperationStack m_op_stack;

    QTemporaryFile m_backup_file;
    std::filesystem::path m_file_path;
    std::filesystem::path m_backup_path;

    std::vector<SegmentInfo> m_segments;
};


#endif // CONTROLLER_H
