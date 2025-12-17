#pragma once

#include "iron/base/entry.hpp"

#include <boost/unordered_map.hpp>

namespace Iron {

    // https://stackoverflow.com/questions/10058606/splitting-a-string-by-a-character

    inline std::vector<std::string> utilGetSubEntryNames(std::string names) {
        std::stringstream ssNames(names);
        std::vector<std::string> out;
        std::string sub;

        while(std::getline(ssNames, sub, '.')) {
            out.push_back(sub);
        }

        return out;
    }

    inline Result<Entry*> utilFindEntryFromMap(boost::unordered_map<std::string, Entry>* map, std::string path, bool createIfNotFound) {
        boost::unordered_map<std::string, Entry>::iterator it;
        Entry* entry = nullptr;
        auto names = utilGetSubEntryNames(path);

        if(names.at(0) == "") {
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        for(it = map->begin(); it != map->end(); it++) {
            if(it->first == names.at(0)) {
                if(names.size() == 1) {
                    return &map->at(it->first);
                }

                entry = &it->second;
                break;
            }
        }

        for(int i = 1; i < names.size(); i++) {
            std::string name = names.at(i);

            if(name == "") {
                return Failure(IRON_RESULT_IMPROPER_FORMAT);
            }

            if(!entry->HasSubEntry(name)) {
                if(!createIfNotFound) {
                    return Failure(IRON_RESULT_FAILED);
                }

                entry->AddSubEntry(new Entry(name, ""));
            }

            auto getEntry = entry->SubEntry(name);
            entry = getEntry.Value();
        }

        return entry;
    }
}