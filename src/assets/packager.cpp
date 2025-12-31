#include "iron/assets/packager.hpp"
#include "iron/fs/fs_map.hpp"

using namespace Iron;

Packager::Packager(std::string location) {
    FileSystem::Map map = FileSystem::Map(location.c_str(), FileSystem::IRON_MAP_READ);

    if(!map.Valid()) {
        return;
    }

    SetMagic(0x60, 0x0D, 0x13, 0x55);
    ResetGrammar();
}