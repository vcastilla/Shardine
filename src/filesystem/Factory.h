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

#ifndef FACTORY_H
#define FACTORY_H

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "FileSystem.h"

namespace fs {

struct Constructor {
    std::string name;
    std::function<bool(const std::filesystem::path&)> mkfs;
    std::function<std::unique_ptr<FileSystem>(const std::filesystem::path&)> ctor;
};

extern std::vector<Constructor> ctors;

std::unique_ptr<FileSystem> from_existing_file(const std::filesystem::path& path);
std::vector<std::string> name_list();

} // namespace fs

#endif // FACTORY_H
