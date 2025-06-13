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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QActionGroup>
#include <QByteArray>
#include <QCheckBox>
#include <QFileDialog>
#include <QMutex>
#include <QStandardItemModel>
#include <QString>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <libassert/assert.hpp>

#include "CreateFS.h"
#include "StructureModel.h"
#include "qt-utils/qtutils.h"
#include "utils/utils.h"

#define CONNECT(sender, signal, recv, slot) ASSERT(connect(sender, signal, recv, slot), "Could not connect QT signal")

constexpr auto status_time = 4000;

void MainWindow::init_hex_edit() {
    m_ui->actionViewASCII->setChecked(m_ui->hexEdit->asciiArea());
    CONNECT(m_ui->actionViewASCII, &QAction::toggled, m_ui->hexEdit, &HexEdit::setAsciiArea);

    m_ui->charGroupTool->setExclusive(true);
    switch (m_ui->hexEdit->defaultChar()) {
        case '.':
            m_ui->actionCharPoint->setChecked(true);
            break;
        case '_':
            m_ui->actionCharUnderscore->setChecked(true);
            break;
        case ' ':
            m_ui->actionCharSpace->setChecked(true);
            break;
        default:
            break;
    }
    CONNECT(m_ui->actionCharPoint, &QAction::triggered, this, [this] { m_ui->hexEdit->setDefaultChar('.'); });
    CONNECT(m_ui->actionCharUnderscore, &QAction::triggered, this, [this] { m_ui->hexEdit->setDefaultChar('_'); });
    CONNECT(m_ui->actionCharSpace, &QAction::triggered, this, [this] { m_ui->hexEdit->setDefaultChar(' '); });
}

void MainWindow::init_status_bar() {
    const auto lb_endian = new QLabel{};
    if constexpr (std::endian::native == std::endian::big)
        lb_endian->setText(tr("Big endian"));
    else if constexpr (std::endian::native == std::endian::little)
        lb_endian->setText(tr("Little endian"));
    else
        lb_endian->setText(tr("Mixed endian"));
    lb_endian->setMinimumWidth(100);
    statusBar()->addPermanentWidget(lb_endian);

    const auto lb_ln_col = new QLabel{};
    lb_ln_col->setMinimumWidth(100);
    statusBar()->addPermanentWidget(lb_ln_col);
    const auto slot_ln_col = [=, this](const qint64 address) {
        const auto bytes_per_line = m_ui->hexEdit->bytesPerLine();
        const auto ln = address / bytes_per_line + 1;
        const auto col = address % bytes_per_line + 1;
        lb_ln_col->setText(tr("Ln %1, Col %2").arg(ln, 1).arg(col, 1));
    };
    CONNECT(m_ui->hexEdit, &HexEdit::currentAddressChanged, this, slot_ln_col);

    const auto lb_address = new QLabel{};
    lb_address->setMinimumWidth(140);
    statusBar()->addPermanentWidget(lb_address);
    const auto slot_address = [=, this](const qint64 address) {
        lb_address->setText(tr("Address 0x%1").arg(m_ui->hexEdit->addressOffset() + address, 1, 16));
    };
    CONNECT(m_ui->hexEdit, &HexEdit::currentAddressChanged, this, slot_address);

    const auto lb_mode = new QLabel{};
    lb_mode->setMinimumWidth(100);
    lb_mode->setText(tr("View mode"));
    statusBar()->addPermanentWidget(lb_mode);
    const auto slot_mode = [=, this](const bool b) { lb_mode->setText(b ? tr("View mode") : tr("Edit mode")); };
    CONNECT(m_ui->hexEdit, &HexEdit::readModeChanged, this, slot_mode);
}

MainWindow::MainWindow(Controller& contr, QWidget* parent) :
    QMainWindow{parent}, m_ui{std::make_unique<Ui::MainWindow>()}, m_contr{contr} {
    m_ui->setupUi(this);

    read_settings();

    enableUI(false);

    CONNECT(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::choose_file_dialog);
    CONNECT(m_ui->actionClose, &QAction::triggered, this, &MainWindow::close_file);

    // QOverload<int> isn't needed in Qt6, but is kept here for backwards compability
    CONNECT(m_ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::update_segment_info);
    CONNECT(m_ui->actionSaveAs, &QAction::triggered, this, &MainWindow::save_as_dialog);
    CONNECT(m_ui->actionSaveChanges, &QAction::triggered, this, &MainWindow::save_changes);
    CONNECT(m_ui->actionSaveHistory, &QAction::triggered, this, &MainWindow::save_history_dialog);

    // Edit mode
    set_edit_mode(false);
    CONNECT(m_ui->btnEdit, &QPushButton::pressed, this, [this] { set_edit_mode(true); });
    CONNECT(m_ui->btnConfirmOp, &QPushButton::pressed, this, &MainWindow::save_op);
    CONNECT(m_ui->btnDiscardOp, &QPushButton::pressed, this, &MainWindow::discard_op);
    utils::qt::set_retain_size(m_ui->btnConfirmOp);
    utils::qt::set_retain_size(m_ui->btnDiscardOp);

    const auto data_changed_slot = [this] {
        if (!m_ui->hexEdit->isModified())
            return;
        m_struct_model.highlight_changes(m_ui->hexEdit->data());
    };
    CONNECT(m_ui->hexEdit, &HexEdit::dataChanged, this, data_changed_slot);

    m_ui->listView->setModel(&m_segments_model);
    CONNECT(m_ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::update_fs_info);

    m_ui->table->setModel(&m_struct_model);
    const auto selection_slot = [this](const QItemSelection& selected, const QItemSelection&) {
        m_struct_model.change_selection(*m_ui->hexEdit, selected);
    };
    CONNECT(m_ui->table->selectionModel(), &QItemSelectionModel::selectionChanged, this, selection_slot);
    utils::qt::set_retain_size(m_ui->table);
    utils::qt::set_retain_size(m_ui->indexLabel);
    utils::qt::set_retain_size(m_ui->spinBox);

    m_contr.connect_undo_view(*m_ui->undoView);
    CONNECT(m_ui->undoView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection&, const QItemSelection&) {
                const auto is_dirty = !m_contr.is_clean();
                m_ui->actionSaveChanges->setEnabled(is_dirty);
                m_ui->actionCheckConsistency->setEnabled(!is_dirty);
                setWindowModified(is_dirty);
                update_fs_info();
            });

    // TODO: Move to Controller
    const auto fsck_slot = [this] {
        QProcess proc;
        proc.start("/usr/sbin/fsck.minix", {QString::fromStdString(m_file_path)});
        if (!proc.waitForFinished()) {
            QMessageBox message_box{QMessageBox::Critical, tr("Error"), tr("Command failed to execute")};
            message_box.setDetailedText(proc.errorString());
            message_box.exec();
            return;
        }

        if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
            QMessageBox::warning(this, tr("Filesystem errors detected"), proc.readAll());
            return;
        }

        QMessageBox::information(this, tr("No errors detected"), tr("No filesystem errors were detected."));
    };
    CONNECT(m_ui->actionCheckConsistency, &QAction::triggered, this, fsck_slot);

    CONNECT(m_ui->actionCreate, &QAction::triggered, this, [this] {
        if (const auto file = CreateFS::createFile(this); !file.isEmpty())
            open_file(file.toStdString());
    });

    utils::qt::set_action(m_ui->actionMountDummy, m_ui->actionMount);
    const auto mount_slot = [this] {
        if (m_contr.is_mounted()) {
            if (!m_contr.umount()) {
                QMessageBox::critical(this, tr("Could not unmount file"),
                                      tr("Failed to unmount.\n"
                                         "Ensure there are no pending operations or open terminals at the mount point "
                                         "and try again."));
                return;
            }
            utils::qt::set_action(m_ui->actionMountDummy, m_ui->actionMount);
        } else {
            const auto dir = QFileDialog::getExistingDirectory(this, tr("Select mount point"), QDir::currentPath());
            if (dir.isEmpty())
                return;
            if (!m_contr.mount(dir)) {
                QMessageBox::critical(this, tr("Could not mount file"),
                                      tr("Failed to mount at the specified directory."));
                return;
            }
            utils::qt::set_action(m_ui->actionMountDummy, m_ui->actionUnmount);
        }
    };
    CONNECT(m_ui->actionMountDummy, &QAction::triggered, this, mount_slot);

    CONNECT(&m_contr, &Controller::unmounted, this,
            [this] { utils::qt::set_action(m_ui->actionMountDummy, m_ui->actionMount); });
    CONNECT(&m_contr, &Controller::file_changed, this, &MainWindow::reload_changes_dialog);

    CONNECT(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about_dialog);
    CONNECT(m_ui->actionAboutQt, &QAction::triggered, this, &QApplication::aboutQt);

    CONNECT(m_ui->actionQuit, &QAction::triggered, this, &QApplication::quit);

    init_hex_edit();
    init_status_bar();
}

MainWindow::~MainWindow() = default;

void MainWindow::update_fs_info() {
    const auto index_list = m_ui->listView->selectionModel()->selectedIndexes();
    if (index_list.isEmpty())
        return;
    const auto row = index_list.first().row();
    const auto segment = m_contr.get_segments().at(row);
    const auto single_elem = segment.is_single_element();

    const auto old_state = m_ui->spinBox->blockSignals(true);
    m_ui->spinBox->setMinimum(segment.min_elem_idx);
    m_ui->spinBox->setMaximum(segment.max_elem_idx);
    if (const auto pair = m_spinbox_cache.find(row); pair != m_spinbox_cache.end())
        m_ui->spinBox->setValue(pair->second);
    else
        m_ui->spinBox->setValue(segment.min_elem_idx);
    m_ui->spinBox->blockSignals(old_state);

    const auto structure = m_contr.get_structure(row, m_ui->spinBox->value());
    display_data(structure);

    m_ui->table->setVisible(!structure.fields.empty());

    m_ui->indexLabel->setText(tr("%1 index:").arg(segment.elem_name));
    m_ui->indexLabel->setVisible(!single_elem);
    m_ui->spinBox->setVisible(!single_elem);
}

/* Reload the displayed data without changing the segment. */
void MainWindow::update_segment_info() {
    const auto index_list = m_ui->listView->selectionModel()->selectedIndexes();
    DEBUG_ASSERT(!index_list.isEmpty());
    const auto segment_idx = index_list.first().row();
    const auto idx = m_ui->spinBox->value();

    const auto structure = m_contr.get_structure(segment_idx, idx);
    display_data(structure);

    m_ui->table->setVisible(!structure.fields.empty());

    m_spinbox_cache.insert_or_assign(segment_idx, idx);
}

void MainWindow::display_data(const fs::Structure& structure) {
    // Display in HexEdit
    const auto [offset, data, fields] = structure;
    m_ui->hexEdit->setData(data);
    m_ui->hexEdit->setAddressOffset(utils::to_signed(offset));
    const auto log16 = [](const auto x) { return static_cast<int>(std::log2(x)) / 4; };
    m_ui->hexEdit->setAddressWidth(std::max(4, 1 + log16(offset)));

    // Display in table
    m_struct_model.setStructure(structure);
}

void MainWindow::enableUI(const bool b) {
    m_ui->centralwidget->setEnabled(b);
    m_ui->actionSaveAs->setEnabled(b);
    m_ui->actionClose->setEnabled(b);
    m_ui->actionMountDummy->setEnabled(b);
    if (b) {
        const auto is_dirty = isWindowModified();
        m_ui->actionCheckConsistency->setEnabled(!is_dirty);
        m_ui->actionSaveChanges->setEnabled(is_dirty);
    } else {
        m_ui->actionCheckConsistency->setEnabled(false);
        m_ui->actionSaveChanges->setEnabled(false);
    }
    update_title();
}

void MainWindow::choose_file_dialog() {
    const auto default_dir = QDir::homePath();
    const auto file_name = QFileDialog::getOpenFileName(this, tr("Choose file"), default_dir);
    if (file_name.isEmpty()) {
        return;
    }
    open_file(file_name.toStdString());
}

void MainWindow::open_file(const std::filesystem::path& file_name) {
    m_contr.close();

    if (!m_contr.open(file_name)) {
        QMessageBox::critical(this, tr("Error"), tr("The file does not contain a supported filesystem."));
        return;
    }

    m_file_path = file_name;

    m_segments_model.setSegments(m_contr.get_segments());
    m_ui->listView->setCurrentIndex(m_segments_model.index(0, 0));

    enableUI(true);
}

void MainWindow::close_file() {
    if (!warn_unsaved_changes())
        return;

    m_contr.close();

    m_struct_model.setStructure({});
    m_segments_model.setSegments({});
    m_file_path.clear();
    m_spinbox_cache.clear();

    update_fs_info();

    enableUI(false);
}

void MainWindow::save_changes() {
    statusBar()->showMessage(tr("Saving changes..."));
    if (!m_contr.save()) {
        QMessageBox::critical(this, tr("Could not save file"), tr("Failed to save the file."));
        statusBar()->clearMessage();
    }
    statusBar()->showMessage(tr("Saved changes to %1").arg(QString::fromStdString(m_file_path.string())), status_time);
}

void MainWindow::reload_changes_dialog() {
    static QMutex dialog_mutex;

    if (!dialog_mutex.tryLock())
        return;

    if (m_contr.are_new_changes_unsaved()) {
        QMessageBox box;
        box.setIcon(QMessageBox::Warning);
        box.setWindowTitle(tr("Reload changes"));
        box.setText(tr("File has been modified on disk. Reload changes?\n"
                       "NOTE: New operations will be added at the last saved point, "
                       "discarding subsequent operations."));

        const auto reload_btn = box.addButton(tr("&Reload"), QMessageBox::ApplyRole);
        box.addButton(QMessageBox::Cancel);
        box.setDefaultButton(reload_btn);
        box.exec();

        if (box.clickedButton() == reload_btn) {
            reload_changes();
        }

        dialog_mutex.unlock();
        return;
    }

    if (m_auto_reload) {
        reload_changes();
        dialog_mutex.unlock();
        return;
    }

    QMessageBox box;
    box.setIcon(QMessageBox::Question);
    box.setWindowTitle(tr("Reload changes"));
    box.setText(tr("File has been modified on disk. Reload changes?"));
    const auto reload_btn = box.addButton(tr("Reload"), QMessageBox::ApplyRole);
    box.addButton(QMessageBox::Cancel);
    box.setDefaultButton(reload_btn);
    const auto cb_text = tr("Reload changes automatically"
                            " if they don't conflict with local changes.");
    box.setCheckBox(new QCheckBox{cb_text});

    box.exec();

    if (box.clickedButton() == reload_btn) {
        m_auto_reload = box.checkBox()->isChecked();
        reload_changes();
    }

    dialog_mutex.unlock();
}

void MainWindow::reload_changes() {
    statusBar()->showMessage(tr("Reloading changes..."));
    m_contr.reload_changes();
    update_fs_info();
    statusBar()->showMessage(tr("Reloaded changes"), status_time);
}

void MainWindow::save_as_dialog() {
    auto path = m_file_path;
    const auto dir = QString::fromStdString(path.remove_filename().string());
    const auto file = QFileDialog::getSaveFileName(this, tr("Save file as"), dir);
    save_as(file.toStdString());
}

void MainWindow::save_as(const std::filesystem::path& file_name) {
    m_contr.save_as(file_name);
    update_title();
}

void MainWindow::save_history_dialog() {
    auto path = m_file_path;
    const auto dir = QString::fromStdString(path.remove_filename().string());
    const auto file = QFileDialog::getSaveFileName(this, tr("Save file as"), dir);
    save_history(file.toStdString());
}

void MainWindow::save_history(const std::filesystem::path& file_name) {
    m_contr.save_history(file_name);
}

void MainWindow::save_op() {
    set_edit_mode(false);

    auto structure = m_struct_model.getStructure();
    structure.raw_data = m_ui->hexEdit->data();
    m_contr.add_change(structure);

    // In case the operation was obsolete, redisplay the data
    display_data(structure);
}

void MainWindow::discard_op() {
    set_edit_mode(false);
    update_fs_info();
}

void MainWindow::about_dialog() {
    QMessageBox box;
    box.setWindowTitle(tr("About %1").arg(m_app_name));
    box.setTextFormat(Qt::RichText);
    box.setText(tr("This is %1 version %2.<br />"
                   "%1 is a simple filesystem viewer and editor, published under the "
                   "<a href=\"%3\">GNU General Public License version 3</a>.<br />"
                   "Learn more about %1 at <a href=\"%4\">%4</a>"
                   "<hr>"
                   "<a href=\"%5\">QHexEdit</a> hex editor widget is &copy; 2010-2025 Winfried Simon, "
                   "licensed under the <a href=\"%6\">LGPL-2.1 license</a>.<br />"
                   "<a href=\"%7\">Icon set</a> is &copy; 2000-2025 JetBrains s.r.o. and contributors, "
                   "licensed under the <a href=\"%8\">Apache-2.0 license</a>.")
                        .arg(project::name)
                        .arg(project::version)
                        .arg("https://www.gnu.org/licenses/gpl-3.0.html")
                        .arg(m_app_link)
                        .arg("https://github.com/Simsys/qhexedit2")
                        .arg("https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html")
                        .arg("https://intellij-icons.jetbrains.design")
                        .arg("https://www.apache.org/licenses/LICENSE-2.0"));
    box.exec();
}

bool MainWindow::is_edit_mode() const {
    return m_edit_mode;
}

void MainWindow::set_edit_mode(const bool b) {
    m_edit_mode = b;
    m_ui->hexEdit->setReadOnly(!b);

    // Manage related buttons
    m_ui->btnEdit->setDisabled(b);
    m_ui->btnConfirmOp->setVisible(b);
    m_ui->btnDiscardOp->setVisible(b);

    // Disable surrounding widgets on edit mode to prevent the user from clicking on them
    m_ui->listView->setDisabled(b);
    m_ui->undoView->setDisabled(b);
    m_ui->menubar->setDisabled(b);
    m_ui->toolBar->setDisabled(b);
    m_ui->spinBox->setDisabled(b);
}

void MainWindow::update_title() {
    if (m_file_path.empty()) {
        setWindowTitle(m_app_name);
        return;
    }

    const auto title = tr("[*]%1 (%2 filesystem)")
                               .arg(QString::fromStdString(m_file_path.filename().string()))
                               .arg(m_contr.fs_name());
    setWindowTitle(title);
}

void MainWindow::save_settings() {
    QSettings settings{m_org_name, m_app_name};

    // Save MainWindow geometry and state
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.endGroup();

    // Save table header sizes
    settings.setValue("table/state", m_ui->table->header()->saveState());

    // Save splitter sizes
    settings.setValue("splitter/state", m_ui->splitter->saveState());
    settings.setValue("splitter_2/state", m_ui->splitter_2->saveState());
    settings.setValue("splitter_3/state", m_ui->splitter_3->saveState());

    // Save HexEdit config
    settings.setValue("hex_edit/ascii", m_ui->hexEdit->asciiArea());
    settings.setValue("hex_edit/char", m_ui->hexEdit->defaultChar());
}

void MainWindow::read_settings() {
    QSettings settings{m_org_name, m_app_name};

    // Restore MainWindow geometry and state
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();

    // Restore table header sizes
    m_ui->table->header()->restoreState(settings.value("table/state").toByteArray());

    // Restore splitter sizes
    m_ui->splitter->restoreState(settings.value("splitter/state").toByteArray());
    m_ui->splitter_2->restoreState(settings.value("splitter_2/state").toByteArray());
    m_ui->splitter_3->restoreState(settings.value("splitter_3/state").toByteArray());

    // Restore HexEdit config
    m_ui->hexEdit->setAsciiArea(settings.value("hex_edit/ascii", true).toBool());
    m_ui->hexEdit->setDefaultChar(settings.value("hex_edit/char", ' ').value<char>());
}

/* Returns false if user did not want to close. */
bool MainWindow::warn_unsaved_changes() {
    if (!is_edit_mode() && m_contr.is_clean())
        return true;
    const auto title_txt = tr("Unsaved changes");
    const auto warn_txt = tr("Do you want to close without saving? Any unapplied operations will be lost.");
    const auto ret = QMessageBox::warning(this, title_txt, warn_txt, QMessageBox::Cancel | QMessageBox::Discard,
                                          QMessageBox::Cancel);
    return ret == QMessageBox::Discard;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!warn_unsaved_changes()) {
        event->ignore();
        return;
    }
    save_settings();
    QMainWindow::closeEvent(event);
}
