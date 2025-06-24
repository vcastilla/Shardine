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

#include "CreateFS.h"
#include "ui_CreateFS.h"

#include <QFileDialog>
#include "filesystem/Factory.h"
#include "qt-utils/qtutils.h"
#include "utils/utils.h"

QString CreateFS::createFile(QWidget* parent) {
    const auto dialog = std::make_unique<CreateFS>(parent);
    if (dialog->exec() == Accepted)
        return dialog->path();
    return {};
}

CreateFS::CreateFS(QWidget* parent) :
    QDialog{parent}, m_ui{std::make_unique<Ui::CreateFS>()},
    m_buttonBox{std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel)} {
    m_ui->setupUi(this);

    connect(m_ui->pathButton, &QPushButton::pressed, this, &CreateFS::createFileDialog);

    for (const auto& [name, mkfs, ctor]: fs::ctors)
        m_ui->fileTypeBox->addItem(QString::fromStdString(name));

    connect(m_buttonBox.get(), &QDialogButtonBox::accepted, this, &CreateFS::accept);
    connect(m_buttonBox.get(), &QDialogButtonBox::rejected, this, &CreateFS::reject);
    m_ui->gridLayout->addWidget(m_buttonBox.get());
}

CreateFS::~CreateFS() = default;

void CreateFS::createFileDialog() {
    const auto default_dir = QDir::homePath();
    const auto file_name = QFileDialog::getSaveFileName(this, tr("Create file"), default_dir);
    if (file_name.isEmpty())
        return;
    setPath(file_name);
}

bool CreateFS::createFileSystem() {
    // Create file
    const auto path = std::filesystem::path{m_ui->pathText->text().toStdString()};
    const auto size_mib = m_ui->fileSizeBox->value();
    const auto size = static_cast<std::streamoff>(1024 * 1024 * size_mib);
    if (!utils::create_file(path, size)) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid file path"));
        return false;
    }

    // Make file system
    const auto idx = m_ui->fileTypeBox->currentIndex();
    if (!fs::ctors.at(idx).mkfs(path)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not create filesystem"));
        return false;
    }

    return true;
}

void CreateFS::accept() {
    if (!createFileSystem())
        return;
    QDialog::accept();
}

QString CreateFS::path() const {
    return m_ui->pathText->text();
}

void CreateFS::setPath(const QString& text) {
    m_ui->pathText->setText(text);
}
