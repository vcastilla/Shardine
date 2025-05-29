#ifndef QTUTILS_H
#define QTUTILS_H

#include <QAction>
#include <QMessageBox>
#include <QObject>
#include <QProcess>

namespace utils::qt {

void set_retain_size(auto widget) {
    auto sp = widget->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    widget->setSizePolicy(sp);
}

inline bool exec(const QString& cmd, const QStringList& args) {
    QMessageBox message_box{QMessageBox::Critical, QObject::tr("Error"),
                            QObject::tr("External command failed to execute")};

    QProcess proc;
    proc.start(cmd, args);
    if (!proc.waitForFinished()) {
        message_box.setDetailedText(proc.errorString());
        message_box.exec();
        return false;
    }

    if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        message_box.setDetailedText(proc.readAllStandardError());
        message_box.exec();
        return false;
    }

    return true;
}

inline void set_action(QAction* dst, const QAction* src) {
    if (!dst || !src)
        return;
    dst->setText(src->text());
    dst->setIcon(src->icon());
    dst->setToolTip(src->toolTip());
    dst->setShortcuts(src->shortcuts());
}

} // namespace utils::qt

#endif // QTUTILS_H
