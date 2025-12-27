#pragma once

#include "iron/result.hpp"
#include "iron/macros.hpp"

#include <iostream>
#include <vector>

namespace Iron {

    class IronDLL Entry {

        std::string name;
        std::vector<std::string> entries;
        std::vector<Entry*> subentries;

        public:

        Entry();
        Entry(std::string name, std::string value);
        Entry(std::string name, std::vector<std::string> values);
        
        void AddEntry(std::string str);
        void SetEntries(std::string str);
        void SetEntries(std::vector<std::string> strs);
        bool RemoveEntry(int idx);
        bool HasEntry(std::string value);
        void AddSubEntry(Entry* entry);
        bool HasSubEntry(std::string name);
        bool RemoveSubEntry(int idx);
        int GetNumEntries();
        int GetNumSubEntries();
        std::string GetName();
        
        std::string String();
        Result<std::string> String(int idx);
        std::vector<std::string> Vector();
        std::vector<Entry*> SubEntries();
        Result<Entry*> SubEntry(std::string name);

        operator std::string();
        operator std::vector<std::string>();

    };

}