#pragma once

#include "iron/result.hpp"
#include "iron/macros.hpp"

#include <iostream>
#include <vector>

namespace Iron {

    class BoolItem;
    class NumberItem;
    class StringItem;
    class ArrayItem;

    enum ItemTypeID {
            IRON_ITEM_ARRAY = 0xD0,
            IRON_ITEM_BOOL = 0xD1,
            IRON_ITEM_GENERIC = 0xD2,
            IRON_ITEM_NUMBER = 0xD3,
            IRON_ITEM_STRING = 0xD4,
    };

    class Archive;

    class IronDLL Item {

        protected:

        Archive* owner;
        std::string name;

        public:

        Item();
        Item(Archive* owner);

        void SetName(std::string name);
        std::string GetName();

        virtual ItemTypeID GetType();
        virtual void Pass(std::vector<char> bytes);

    };

    class IronDLL Archive {

        bool isChildArchive = false;
        std::vector<Archive*> childArchives;
        std::vector<Item*> items;
        std::string name;
        std::string type;

        public:

        BoolItem* RetrieveBool(std::string name, bool def);
        NumberItem* RetrieveNumber(std::string name, double def);
        NumberItem* RetrieveNumber(std::string name, float def);
        NumberItem* RetrieveNumber(std::string name, int def);
        StringItem* RetrieveString(std::string name, std::string def);
        ArrayItem* RetrieveArray(std::string name);

        void AttachItem(Item* item);
        void AttachArchive(Archive* archive);
        void DetachArchive(std::string name);

        void SetName(std::string name);
        std::string GetName();
        void SetType(std::string type);
        std::string GetType();

        void Clean();

    };

    class IronDLL ArrayItem : public Item {

        std::vector<Item*> items;

        public:

        ArrayItem();
        ArrayItem(Archive* owner);

        void Append(Item* item);
        void Remove(int idx);
        void Pop();
        int Size();
        Result<Item*> At(int idx);

    };

    class IronDLL BoolItem : public Item {

        bool value;

        public:

        BoolItem();
        BoolItem(Archive* owner);
        BoolItem(Archive* owner, bool value);
        
        void Pass(std::vector<char> bytes);

        BoolItem& operator=(bool rhs);
        operator bool();

    };

    class IronDLL NumberItem : public Item {

        double value;

        public:

        NumberItem();
        NumberItem(Archive* owner);
        NumberItem(Archive* owner, double value);

        void Pass(std::vector<char> bytes);

        NumberItem& operator=(double rhs);
        operator double();

    };

    class IronDLL StringItem : public Item {

        std::string value;

        public:

        StringItem();
        StringItem(Archive* owner);
        StringItem(Archive* owner, std::string value);

        void Pass(std::string bytes);

        StringItem& operator=(std::string rhs);
        operator std::string();
        operator const char*();
        
    };

}
