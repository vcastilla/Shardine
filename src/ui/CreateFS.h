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
