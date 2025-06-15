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

#include "Controller.h"

#include <QTimer>
#include <libassert/assert.hpp>
#include "filesystem/Factory.h"
#include "filesystem/Segment.h"
#include "qt-utils/qtutils.h"

#define CONNECT(sender, signal, recv, slot) ASSERT(connect(sender, signal, recv, slot), "Could not connect QT signal")

bool Controller::open(const std::filesystem::path& file_name) {
    close();

    m_file_path = file_name;

    m_file_system = fs::Factory::create(m_file_path);
    if (!m_file_system) {
        return false;
    }

    generate_segments();

    backup();

    m_device = std::make_unique<MountableDevice>(m_file_path);
    CONNECT(m_device.get(), &MountableDevice::stateChanged, this, &Controller::mount_status_changed);
    CONNECT(m_device.get(), &MountableDevice::mounted, this, &Controller::mounted);
    CONNECT(m_device.get(), &MountableDevice::unmounted, this, &Controller::unmounted);

    m_watcher.addPath(QString::fromStdString(m_file_path));
    CONNECT(&m_watcher, &QFileSystemWatcher::fileChanged, this, [this] {
        if (!exists(m_file_path))
            return;
        if (utils::file_equal(m_file_path, m_backup_path))
            return;
        emit file_changed();
    });

    return true;
}

bool Controller::save() {
    if (!m_file_system)
        return false;

    bool ret = true;
    // Block signals related to file updates
    const auto old_state = m_watcher.blockSignals(true);

    // Write to file
    for (const auto& [addr, structure]: m_op_stack.dataContainer())
        m_file_system->write(addr, structure.raw_data);
    m_file_system->flush();

    m_op_stack.setClean();

    backup();

    // Remount device to reflect changes
    if (m_device->is_mounted() && !m_device->remount())
        ret = false;

    // Save history
    // auto path = m_file_path;
    // path.replace_extension(utils::now_to_str("%F")).concat(".log");
    // save_history(path);

    // Block QFileSystemWatcher signals for one more millisecond
    if (!old_state)
        QTimer::singleShot(1, nullptr, [this] { m_watcher.blockSignals(false); });
    return ret;
}

bool Controller::save_as(const std::filesystem::path& file_name) {
    std::filesystem::copy_file(m_file_path, file_name, std::filesystem::copy_options::overwrite_existing);
    m_file_path = file_name;
    return save();
}

bool Controller::save_history(const std::filesystem::path& file_name) {
    return m_op_stack.saveHistory(file_name);
}

bool Controller::close() {
    m_device.reset();
    m_file_system.reset();
    m_fs_backup.reset();
    if (!m_file_path.empty())
        m_watcher.removePath(QString::fromStdString(m_file_path));

    m_op_stack.clear();
    m_file_path.clear();
    return true;
}

QString Controller::fs_name() const {
    return QString::fromStdString(m_file_system->name());
}

const std::vector<SegmentInfo>& Controller::get_segments() {
    return m_segments;
}

fs::Structure Controller::get_structure(const unsigned segment_idx, const unsigned elem_idx) {
    if (!m_file_system)
        return {};
    const auto segment = m_file_system->segments().at(segment_idx);
    return m_op_stack.getStructure(segment, elem_idx);
}

void Controller::connect_undo_view(QUndoView& view) {
    m_op_stack.connectUndoView(view);
}

void Controller::add_change(const fs::Structure& structure) {
    m_op_stack.addOperation(*m_file_system, structure);
}

bool Controller::mount(const QString& str) {
    return m_device && m_device->mount(str.toStdString());
}

bool Controller::is_mounted() const {
    return m_device && m_device->is_mounted();
}

bool Controller::umount() {
    return m_device && m_device->umount();
}

void Controller::reload_changes() {
    // TODO: Don't show error dialog if failed
    utils::qt::exec("/bin/sync", {QString::fromStdString(m_file_path)});

    if (utils::file_equal(m_file_path, m_backup_path))
        return;

    m_file_system = fs::Factory::create(m_file_path);
    if (!m_file_system)
        return;

    m_op_stack.updateChanges(*m_fs_backup, *m_file_system);
    generate_segments();
    backup();
}

bool Controller::is_clean() const {
    return m_op_stack.isClean();
}

bool Controller::are_new_changes_unsaved() const {
    return m_op_stack.areNewChangesUnsaved();
}

void Controller::generate_segments() {
    m_segments.resize(0);
    for (const auto& s: m_file_system->segments()) {
        SegmentInfo info{s.name,
                         "",
                         s.element_name,
                         utils::cast<int>(s.min_element_index),
                         utils::cast<int>(s.max_element_index),
                         s.size};
        m_segments.emplace_back(info);
    }
}

void Controller::backup() {
    if (m_backup_path.empty()) {
        m_backup_file.open(); // Create temporary file
        m_backup_path = m_backup_file.fileName().toStdString();
        m_backup_file.close();
    }
    std::filesystem::copy_file(m_file_path, m_backup_path, std::filesystem::copy_options::overwrite_existing);
    m_fs_backup = fs::Factory::create(m_backup_path);
}
