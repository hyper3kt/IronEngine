#include "fs_map.hpp"

#if defined(__linux__) || defined(LINUX) || defined(__unix__) || defined(UNIX)
#define Linux
#endif

#if defined(_WIN32) || defined(WIN32)
#define Windows
#endif

#ifdef Linux

#endif

#ifdef Windows
#include <Windows.h>
#endif

using namespace Iron;
using namespace FileSystem;

// https://stackoverflow.com/questions/68368291/mapping-files-into-virtual-memory-in-c-on-windows

Map::Map(const char* path, MapPermissions perms) {
    this->perms = perms;
    #ifdef Windows
    DWORD creationDesiredAccess = perms == IRON_MAP_READ ? GENERIC_READ : GENERIC_WRITE;
    fileHandle = CreateFileA(path, creationDesiredAccess, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(fileHandle == INVALID_HANDLE_VALUE) {
        Close();
        return;
    }

    LARGE_INTEGER getFileSize;

    if(!GetFileSizeEx(fileHandle, &getFileSize)) {
        Close();
        return;
    }

    fileSize = (size_t) getFileSize.QuadPart;

    if(fileSize == 0) {
        Close();
        return;
    }

    DWORD protect = perms == IRON_MAP_READ ? PAGE_READONLY : PAGE_READWRITE;
    mapHandle = CreateFileMapping(fileHandle, NULL, protect, 0, 0, NULL);

    if(mapHandle == NULL) {
        Close();
        return;
    }

    DWORD mapDesiredAccess = perms == IRON_MAP_READ ? FILE_MAP_READ : FILE_MAP_WRITE;
    LPVOID view = MapViewOfFile(mapHandle, mapDesiredAccess, 0, 0, 0);

    if(view == NULL) {
        Close();
        return;
    }

    valid = true;
    map = (char*) view;
    #endif
}

void Map::Close() {
    #ifdef Windows
    if(mapHandle != nullptr) {
        CloseHandle(mapHandle);
    }

    if(fileHandle != nullptr) {
        CloseHandle(fileHandle);
    }
    
    if(valid) {
        UnmapViewOfFile((LPVOID) map);
    }
    #endif
}

bool Map::Valid() {
    return valid;
}

// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c

size_t Map::Size() {
    return fileSize;
}

char* Map::Get() {
    return map;
}

char Map::Get(int i) {
    return map[i];
}

bool Map::Set(int i, char c) {
    if(!Valid() || perms == IRON_MAP_READ || Size() <= i) {
        return false;
    }

    map[i] = c;
    return true;
}