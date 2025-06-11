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
#include <vector>
#include "FileSystem.h"

namespace fs {

class Factory {
public:
    static std::unique_ptr<FileSystem> create(const std::filesystem::path& path);

private:
    using Constructor = std::function<std::unique_ptr<FileSystem>(const std::filesystem::path&)>;
    static std::vector<Constructor> ctors;
};

} // namespace fs

#endif // FACTORY_H
