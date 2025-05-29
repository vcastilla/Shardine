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
