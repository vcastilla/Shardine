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
#include "MinixAdapter.h"

namespace fs {

std::vector<Factory::Constructor> Factory::ctors{
        // MinixAdapter
        [](const std::filesystem::path& path) -> std::unique_ptr<FileSystem> {
            try {
                return std::make_unique<MinixAdapter>(path);
            } catch (...) {
                return nullptr;
            }
        },
};

std::unique_ptr<FileSystem> Factory::create(const std::filesystem::path& path) {
    for (const auto& ctor: ctors) {
        if (auto ptr = ctor(path); ptr)
            return ptr;
    }
    return nullptr;
}

} // namespace fs
