#pragma once

#include "result.hpp"

#include <iostream>
#include <vector>
#include <boost/unordered_map.hpp>

namespace Iron {

    enum ConfigEntryType {
        IRON_ENTRY_STRING,
        IRON_ENTRY_STRING_LIST,
        IRON_ENTRY_DELETE,
    };

    struct ConfigEntry {

        union ConfigEntryData {
            std::string string;
            std::vector<std::string> list;

            inline ConfigEntryData() {}
            inline ~ConfigEntryData() {}

            inline ConfigEntryData(ConfigEntryData& other) {
                *this = other;
            }

            inline ConfigEntryData operator=(ConfigEntryData& other) {
                return other;
            }
        };

        union ConfigEntryData data;
        ConfigEntryType type;
        std::string name;

        inline ConfigEntry() {}
        inline ConfigEntry(std::string data, ConfigEntryType type, std::string name) {
            this->data.string = data;
            this->type = type;
            this->name = name;
        }

        inline ConfigEntry(std::vector<std::string> data, ConfigEntryType type, std::string name) {
            this->data.list = data;
            this->type = type;
            this->name = name;
        }

        inline ConfigEntry operator=(const ConfigEntry& other) {
            if(other.type == IRON_ENTRY_STRING) {
                return ConfigEntry(other.data.string, IRON_ENTRY_STRING, other.name);
            } else return ConfigEntry(other.data.list, IRON_ENTRY_STRING_LIST, other.name);
        }

        inline ConfigEntry(ConfigEntry& other) {
            this->data = other.data;
            this->name = other.name;
            this->type = other.type;
        }

    };

    class Config {

        boost::unordered_map<std::string, ConfigEntry> map;
        boost::unordered_map<std::string, ConfigEntry> changes;
        std::string path;

        public:
        Config(std::string path);
        static Config CreateEmpty(std::string name);
        static Config CreateDefaultSettings();

        Result<EngineResult> Load();
        Result<ConfigEntry> GetEntry(std::string name);
        void SetEntry(std::string name, std::string value);
        void SetEntry(std::string name, std::vector<std::string> value);
        void RemoveEntry(std::string name);
        bool HasEntry(std::string name);
        bool HasEntryInFile(std::string name);
        Result<EngineResult> SaveChanges();
    };

}