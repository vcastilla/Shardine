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
