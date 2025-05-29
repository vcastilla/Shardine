#include "CreateFS.h"
#include "ui_CreateFS.h"

#include <QFileDialog>
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
    const auto cmd = QString{"/usr/sbin/mkfs.minix"};
    const auto args = QStringList{"-3", path.c_str()};
    return utils::qt::exec(cmd, args);
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
