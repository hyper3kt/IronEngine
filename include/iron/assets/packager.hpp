#pragma once

#include "iron/result.hpp"
#include "iron/macros.hpp"
#include "iron/fs/fs_grammar.hpp"

#include <boost/unordered_map.hpp>

#include <iostream>

namespace Iron {

    class IronDLL Asset;

    struct IronDLL AssetEntry {
        long long size;
        long long offset;
    };

    class IronDLL Packager : public FileSystem::Grammar {

        bool loadedManifest = false;
        bool hasAssetPath = false;
        std::string assetPath = "";
        boost::unordered_map<std::string, AssetEntry> manifest;

        void AddManifest(std::vector<FileSystem::Element> elements);

        public:

        Packager(std::string location);
        bool LoadedManifest();
        bool HasAssetPath();
        std::string GetAssetPath();
        Asset* GetAsset(std::string path);
        
    };

}