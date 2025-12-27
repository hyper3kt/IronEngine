#include "iron/base/config.hpp"
#include "config_parser.hpp"
#include "entry_util.hpp"
#include "iron/fs/fs_map.hpp"

using namespace Iron;

Entry::Entry() {}

Entry::Entry(std::string name, std::string value) {
    this->name = name;
    entries.push_back(value);
}

Entry::Entry(std::string name, std::vector<std::string> values) {
    this->name = name;
    entries = values;
}

void Entry::AddEntry(std::string value) {
    entries.push_back(value);
}

void Entry::SetEntries(std::string str) {
    entries.clear();
    entries.push_back(str);
}

void Entry::SetEntries(std::vector<std::string> strs) {
    entries = strs;
}

bool Entry::HasEntry(std::string value) {
    for(int i = 0; i < entries.size(); i++) {
        if(entries.at(i) == value) {
            return true;
        }
    }

    return false;
}

bool Entry::RemoveEntry(int idx) {
    if(idx >= GetNumEntries()) {
        return false;
    }

    entries.erase(entries.begin() + idx);
    return true;
}

void Entry::AddSubEntry(Entry* entry) {
    subentries.push_back(entry);
}

bool Entry::HasSubEntry(std::string name) {
    for(int i = 0; i < subentries.size(); i++) {
        if(subentries.at(i)->GetName() == name) {
            return true;
        }
    }

    return false;
}

bool Entry::RemoveSubEntry(int idx) {
    if(idx >= GetNumSubEntries()) {
        return false;
    }

    subentries.erase(subentries.begin() + idx);
    return true;
}

int Entry::GetNumEntries() {
    return entries.size();
}

int Entry::GetNumSubEntries() {
    return subentries.size();
}

std::string Entry::GetName() {
    return name;
}

std::string Entry::String() {
    return entries.at(0);
}

Result<std::string> Entry::String(int idx) {
    if(idx >= GetNumEntries()) {
        return Failure(IRON_RESULT_TOO_BIG);
    }

    return entries.at(idx);
}

std::vector<std::string> Entry::Vector() {
    return entries;
}

std::vector<Entry*> Entry::SubEntries() {
    return subentries;
}

Result<Entry*> Entry::SubEntry(std::string name) {
    for(int i = 0; i < subentries.size(); i++) {
        Entry* sub = subentries.at(i);

        if(sub->GetName() == name) {
            return sub;
        }
    }

    return Failure(IRON_RESULT_NONEXISTENT_REQUEST);
}

Entry::operator std::string() {
    return String();
}

Entry::operator std::vector<std::string>() {
    return Vector();
}

Result<EngineResult> Config::LoadConfig(std::string path) {
    FileSystem::Map fileMap = FileSystem::Map(path.c_str(), FileSystem::IRON_MAP_READ);

    if(!fileMap.Valid()) {
        return Failure(IRON_RESULT_NONEXISTENT_FILE);
    }

    std::string line;
    std::vector<ConfigParser::Token> tokens = ConfigParser::GetTokens(fileMap.Get());
    fileMap.Close();

    ConfigParser::Parser parser;
    auto getMap = parser.GetMap(tokens);

    if(!getMap.Success()) {
        return getMap.Fail();
    }

    entries = getMap.Value();
    
    return IRON_RESULT_LOADED;
}

Result<Entry*> Config::GetEntry(std::string name, bool createIfNotFound = false) {
    return utilFindEntryFromMap(&entries, name, createIfNotFound);
}

void Config::SetEntry(std::string name, std::string value) {
    auto getName = utilFindEntryFromMap(&entries, name, true);

    if(!getName.Success()) {
        return;
    }

    getName.Value()->SetEntries(value);
}

void Config::SetEntry(std::string name, std::vector<std::string> value) {
    auto getName = utilFindEntryFromMap(&entries, name, true);

    if(!getName.Success()) {
        return;
    }

    getName.Value()->SetEntries(value);
}

bool Config::HasEntry(std::string name) {
    return utilFindEntryFromMap(&entries, name, false).Success();
}