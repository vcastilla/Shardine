#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QFileSystemWatcher>
#include <QString>
#include <memory>
#include <vector>

#include "OperationStack.h"
#include "SegmentInfo.h"
#include "filesystem/FileSystem.h"
#include "qt-utils/MountableDevice.h"
#include "qt-utils/Operation.h"

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

    std::filesystem::path m_file_path;
    std::filesystem::path m_backup_path;

    std::vector<SegmentInfo> m_segments;
};


#endif // CONTROLLER_H
