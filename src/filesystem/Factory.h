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
