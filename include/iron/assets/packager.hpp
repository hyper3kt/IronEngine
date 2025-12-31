#pragma once

#include "iron/result.hpp"
#include "iron/fs/fs_grammar.hpp"

#include <iostream>

namespace Iron {

    class Asset;

    class Packager : public FileSystem::Grammar {

        public:

        Packager(std::string location);
        Result<EngineResult> LoadRequiredAssets();
        
    };

}