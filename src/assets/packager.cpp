#include "iron/assets/packager.hpp"
#include "iron/fs/fs_map.hpp"

using namespace Iron;

enum GrammarDefinitions {
    PACKAGER_MANIFEST_BIN,
    PACKAGER_RAW_ASSETS_PATH,
    PACKAGER_MANIFEST_ITEM,
    PACKAGER_MANIFEST_ITEM_NAME,
    PACKAGER_MANIFEST_ITEM_SIZE,
    PACKAGER_MANIFEST_ITEM_OFFSET,
};

std::string utilVecUcharToString(std::vector<uchar> str) {
    std::string ret = "";

    for(int i = 0; i < str.size(); i++) {
        ret += str.at(i);
    }

    return ret;
}

long long utilVecUcharToNumber(std::vector<uchar> str) {
    long long ret;
    memcpy(&ret, str.data(), sizeof(long long));
    return ret;
}

void Packager::AddManifest(std::vector<FileSystem::Element> elements) {
    std::string name = "";
    AssetEntry ae;

    if(elements.size() != 3) {
        return;
    }

    for(int i = 0; i < elements.size(); i++) {
        auto element = elements.at(i);

        switch(element.type) {
            case PACKAGER_MANIFEST_ITEM_NAME:
            name = utilVecUcharToString(element.data);
            break;
            case PACKAGER_MANIFEST_ITEM_SIZE:
            ae.size = utilVecUcharToNumber(element.data);
            break;
            case PACKAGER_MANIFEST_ITEM_OFFSET:
            ae.offset = utilVecUcharToNumber(element.data);
            break;
        }
    }

    manifest[name] = ae;
}

Packager::Packager(std::string location) {
    FileSystem::Map map = FileSystem::Map(location.c_str(), FileSystem::IRON_MAP_READ);

    if(!map.Valid()) {
        return;
    }

    SetMagic(0x60, 0x0D, 0x13, 0x55);
    ResetGrammar();

    auto expectAssetsPath = ElementExpectation(PACKAGER_RAW_ASSETS_PATH, FileSystem::IRON_GRAMMAR_STRING);
    auto expectManifestName = ElementExpectation(PACKAGER_MANIFEST_ITEM_NAME, FileSystem::IRON_GRAMMAR_STRING);
    auto expectManifestOffset = ElementExpectation(PACKAGER_MANIFEST_ITEM_OFFSET, FileSystem::IRON_GRAMMAR_BYTES, 8);
    auto expectManifestSize = ElementExpectation(PACKAGER_MANIFEST_ITEM_SIZE, FileSystem::IRON_GRAMMAR_BYTES, 8);
    FileSystem::ElemExp itemList[] = {expectManifestName, expectManifestOffset, expectManifestSize};
    auto expectManifestItem = ElementExpectation(PACKAGER_MANIFEST_ITEM, sizeof(itemList), itemList);
    FileSystem::ElemExp expectElements[] = {expectAssetsPath, expectManifestItem};
    DefineBinElements(PACKAGER_MANIFEST_BIN, sizeof(expectElements), expectElements);

    auto result = Parse(&map);
    loadedManifest = result.Success();

    map.Close();

    if(!loadedManifest) {
        return;
    }

    if(parsedBins.size() == 0) {
        return;
    }

    auto bin = parsedBins.at(0);
    auto elements = bin.elements;

    for(int i = 0; i < elements.size(); i++) {
        auto element = elements.at(i);

        switch(element.type) {
            case PACKAGER_RAW_ASSETS_PATH:
            hasAssetPath = true;
            assetPath = utilVecUcharToString(element.data);
            break;
            case PACKAGER_MANIFEST_ITEM:
            AddManifest(element.composites);
            break;
        }
    }

    if(!hasAssetPath) {
        return;
    }

    
}

bool Packager::LoadedManifest() {
    return loadedManifest;
}

bool Packager::HasAssetPath() {
    return hasAssetPath;
}

std::string Packager::GetAssetPath() {
    return assetPath;
}