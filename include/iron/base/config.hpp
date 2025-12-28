#pragma once

#include "iron/result.hpp"
#include "iron/base/entry.hpp"
#include "iron/macros.hpp"

#include <iostream>
#include <vector>

#include <boost/unordered_map.hpp>

namespace Iron {

    class IronDLL Config {

        boost::unordered_map<std::string, Entry> entries;
        std::string configPath;

        public:

        Result<EngineResult> LoadConfig(std::string path);
        Result<Entry*> GetEntry(std::string entry, bool createIfNotFound);
        void SetEntry(std::string name, std::string value);
        void SetEntry(std::string name, std::vector<std::string> value);
        bool HasEntry(std::string entry);
        bool SaveChanges();

    };

}