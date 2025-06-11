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

#ifndef CREATEFS_H
#define CREATEFS_H

#include <QDialog>
#include <QDialogButtonBox>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class CreateFS;
}
QT_END_NAMESPACE

class CreateFS final : public QDialog {
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath)

public:
    static QString createFile(QWidget* parent);

    explicit CreateFS(QWidget* parent = nullptr);
    ~CreateFS() override;
    bool createFileSystem();
    void accept() override;

    QString path() const;
    void setPath(const QString& text);

private slots:
    void createFileDialog();

private:
    std::unique_ptr<Ui::CreateFS> m_ui;
    std::unique_ptr<QDialogButtonBox> m_buttonBox;
};


#endif // CREATEFS_H
