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

#ifndef MOUNTABLE_DEVICE_H
#define MOUNTABLE_DEVICE_H

#include <QObject>
#include <QProcess>
#include <QTextStream>
#include <filesystem>
#include <utility>

class MountableDevice final : public QObject {
    Q_OBJECT
public:
    explicit MountableDevice(std::filesystem::path device) : m_device{std::move(device)} {
        connect(&m_findmnt, &QProcess::finished, this, [this](int, const QProcess::ExitStatus exit_status) {
            if (exit_status == QProcess::NormalExit) {
                m_mounted = false;
                notify();
            }
        });
    }

    ~MountableDevice() override {
        umount();
        m_sh.closeWriteChannel();
        m_findmnt.close();
        m_sh.close();
    }

    bool mount(const std::filesystem::path& mount_point) {
        if (m_mounted)
            return false;
        if (!is_regular_file(m_device))
            return false;
        if (!is_directory(mount_point))
            return false;
        m_mount_point = mount_point;
        const auto dev = QString::fromStdString(m_device.string());
        const auto dir = QString::fromStdString(m_mount_point.string());

        if (m_sh.state() == QProcess::NotRunning) {
            m_sh.start("/bin/pkexec", {"/bin/sh"});
            m_sh.waitForStarted();
        }

        if (!exec("/bin/mount", {dev, dir}))
            return false;

        m_mounted = true;

        // Possible poll options are mount, umount, remount and move.
        // It is already mounted, so we don't check for mount.
        // We don't care if it was remounted or moved, as it is still mounted and can be unmounted with the device name.
        // Thus, we only poll for unmounts.
        m_findmnt.start("/bin/findmnt", {"--poll=umount", "--mountpoint", dir, "--noheadings", "--first-only"});

        notify();
        return true;
    }

    bool umount() {
        if (!m_mounted || !is_regular_file(m_device))
            return false;
        const auto dev = QString::fromStdString(m_device.string());
        if (!exec("/bin/umount", {dev}))
            return false;

        m_mounted = false;

        notify();
        return true;
    }

    bool remount() {
        if (!m_mounted || !is_regular_file(m_device) || !is_directory(m_mount_point))
            return false;
        const auto dev = QString::fromStdString(m_device.string());
        const auto dir = QString::fromStdString(m_mount_point.string());

        const auto old_state = m_findmnt.blockSignals(true);

        if (!exec("/bin/umount", {dir}))
            goto bad;

        m_findmnt.kill();
        m_findmnt.waitForFinished();

        if (!exec("/bin/mount", {dev, dir}))
            goto bad;

        m_findmnt.blockSignals(old_state);
        m_findmnt.start("/bin/findmnt", {"--poll=umount", "--mountpoint", dir, "--noheadings", "--first-only"});
        return true;

    bad:
        m_findmnt.blockSignals(old_state);
        return false;
    }

    bool is_mounted() const {
        return m_mounted;
    }

signals:
    void stateChanged(bool mounted);
    void mounted();
    void unmounted();

private:
    void notify() {
        if (m_mounted) {
            emit stateChanged(true);
            emit mounted();
        } else {
            emit stateChanged(false);
            emit unmounted();
        }
    }

    bool exec(const QString& cmd, const QStringList& args) {
        m_cmd_stream << cmd;
        for (const auto& arg: args)
            m_cmd_stream << ' ' << arg;

        m_cmd_stream << ";printf '\\n'$?'\\n'" << Qt::endl;
        m_sh.waitForReadyRead();
        const auto output = m_cmd_stream.readAll();
        return output.endsWith("\n0\n");
    }

    bool m_mounted{false};
    std::filesystem::path m_device;
    std::filesystem::path m_mount_point;
    QProcess m_sh;
    QTextStream m_cmd_stream{&m_sh};
    QProcess m_findmnt;
};


#endif // MOUNTABLE_DEVICE_H
