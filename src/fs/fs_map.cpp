#include "iron/fs/fs_map.hpp"
#include "iron/macros.hpp"

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

void Map::Write(const char* str, size_t strSize, unsigned int offset) {
    if(strSize + offset > fileSize) {
        return;
    }

    for(int i = 0; i < strSize; i++) {
        map[i + offset] = str[i];
    }
}

void Map::Erase() {
    for(int i = 0; i < fileSize; i++) {
        map[i] = 0x0;
    }
}

size_t Map::GetCharsConsumed() {
    return consumed;
}

Result<char> Map::Peek() {
    if(consumed >= Size()) {
        return Failure(IRON_RESULT_TOO_BIG);
    }

    return map[consumed];
}

Result<char> Map::Consume() {
    if(consumed >= Size()) {
        return Failure(IRON_RESULT_TOO_BIG);
    }

    return map[consumed++];
}

void Map::Rewind() {
    if(consumed > 0) {
        consumed--;
    }
}
