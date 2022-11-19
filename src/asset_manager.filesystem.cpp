#include <planet/asset_manager.hpp>

#include <fstream>


std::vector<std::byte> planet::asset_manager::file_data(
        std::filesystem::path const &fn,
        felspar::source_location const &loc) const {
    auto const pathname = find_path(fn, loc);
    std::vector<std::byte> d(std::filesystem::file_size(pathname));
    std::ifstream file{pathname};
    file.read(reinterpret_cast<char *>(d.data()), d.size());
    return d;
}
