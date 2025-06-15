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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <memory>
#include <unordered_map>
#include <utility>
#include "SegmentModel.h"
#include "StructureModel.h"
#include "contr/Controller.h"
#include "contr/OperationStack.h"
#include "project.h"


QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Controller& contr, QWidget* parent = nullptr);

    ~MainWindow() override;

private slots:
    void update_fs_info();
    void update_segment_info();
    void enableUI(bool b);

    void choose_file_dialog();
    void open_file(const std::filesystem::path& file_name);
    bool close_file();

    void save_changes();
    void reload_changes_dialog();
    void reload_changes();
    void save_as_dialog();
    void save_as(const std::filesystem::path& file_name);
    void save_history_dialog();
    void save_history(const std::filesystem::path& file_name);

    void save_op();
    void discard_op();

    static void about_dialog();

private:
    void init_hex_edit();
    void init_status_bar();

    void display_data(const fs::Structure& structure);

    bool is_edit_mode() const;
    void set_edit_mode(bool b);

    void update_title();

    void save_settings();
    void read_settings();

    bool warn_unsaved_changes();
    void closeEvent(QCloseEvent* event) override;

    const std::unique_ptr<Ui::MainWindow> m_ui;

    Controller& m_contr;

    // Models
    StructureModel m_struct_model;
    SegmentModel m_segments_model;

    // State
    bool m_edit_mode{};
    std::filesystem::path m_file_path;
    std::unordered_map<int, int> m_spinbox_cache; // Used to remember spinboxes' positions

    // Preferences
    bool m_auto_reload{};

    // Constants used for settings storage
    static constexpr auto m_app_name = project::name;
    static constexpr auto m_app_version = project::version;
    static constexpr auto m_org_name = "vcastilla";
    static constexpr auto m_app_link = "https://github.com/vcastilla/Shardine";
};


#endif // MAINWINDOW_H
