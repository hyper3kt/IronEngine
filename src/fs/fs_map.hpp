#pragma once

#include <iostream>

namespace Iron {

    namespace FileSystem {

        enum MapPermissions {
            IRON_MAP_READ,
            IRON_MAP_READWRITE,
        };

        class Map {

            char* map;
            void* fileHandle = nullptr;
            void* mapHandle = nullptr;
            MapPermissions perms;
            size_t fileSize;
            bool valid = false;

            public:

            Map(const char* path, MapPermissions perms);

            void Close();
            size_t Size();
            bool Valid();

            char* Get();
            char Get(int i);
            bool Set(int i, char c);

        };

    }
    
}