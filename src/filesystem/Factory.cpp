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

#include "Factory.h"

#include <algorithm>
#include "MinixAdapter.h"
#include "qt-utils/qtutils.h"

namespace fs {

std::vector<Constructor> ctors{
        // MinixAdapter
        {MinixAdapter::fs_name,
         [](const std::filesystem::path& path) {
             const auto cmd = QString{"/usr/sbin/mkfs.minix"};
             const auto args = QStringList{"-3", QString::fromStdString(path)};
             return utils::qt::exec(cmd, args);
         },

         [](const std::filesystem::path& path) -> std::unique_ptr<FileSystem> {
             try {
                 return std::make_unique<MinixAdapter>(path);
             } catch (...) {
                 return nullptr;
             }
         }},
};

std::unique_ptr<FileSystem> from_existing_file(const std::filesystem::path& path) {
    for (const auto& [name, mkfs, ctor]: ctors) {
        if (auto ptr = ctor(path); ptr)
            return ptr;
    }
    return nullptr;
}

std::vector<std::string> name_list() {
    std::vector<std::string> names(ctors.size());
    std::ranges::transform(ctors, names.begin(), [](const Constructor& ctor) { return ctor.name; });
    return names;
}

} // namespace fs
