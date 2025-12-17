#pragma once

#include "iron/result.hpp"

#include <iostream>

namespace Iron {

    class Asset;

    class Packager {

        public:

        Packager(std::string location);
        Result<EngineResult> LoadRequiredAssets();
        
    };

}