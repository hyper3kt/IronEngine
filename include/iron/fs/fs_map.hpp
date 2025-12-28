#pragma once

#include "iron/result.hpp"
#include "iron/macros.hpp"

#include <iostream>

namespace Iron {

    namespace FileSystem {

        enum MapPermissions {
            IRON_MAP_READ,
            IRON_MAP_READWRITE,
        };

        class IronDLL Map {

            char* map;
            void* fileHandle = nullptr;
            void* mapHandle = nullptr;
            MapPermissions perms;
            size_t fileSize;
            size_t consumed = 0;
            bool valid = false;

            public:

            Map(const char* path, MapPermissions perms);

            void Close();
            size_t Size();
            bool Valid();

            char* Get();
            void Write(const char* str, size_t strSize, unsigned int offset = 0);
            void Erase();

            size_t GetCharsConsumed();
            Result<char> Consume();
            Result<char> Peek();
            void Rewind();

        };

    }
    
}