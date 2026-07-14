#pragma once

#include <string>
#include <filesystem>

namespace snow::core {

    inline const std::filesystem::path base_filesystem_path = std::filesystem::current_path();
    inline const std::string base_path = base_filesystem_path.string() + std::string("/../");
};