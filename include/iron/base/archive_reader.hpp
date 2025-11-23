#pragma once

#include "iron/base/archive.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <fstream>

namespace Iron {

    class ArchiveReader {

        enum MagicNumbers {
            IRON_AR_END_STRING = 0x00,
            IRON_AR_BEGIN_ARCHIVE = 0x10,
            IRON_AR_END_ARCHIVE = 0x11,
            IRON_AR_TYPE = 0x12,
            IRON_AR_BEGIN_ENTRIES = 0x13,
            IRON_AR_END_ENTRIES = 0x14,
            IRON_AR_BEGIN_ARRAY = 0x15,
            IRON_AR_END_ARRAY = 0x16,
            IRON_AR_BEGIN_SUBENTRIES = 0x17,
            IRON_AR_END_SUBENTRIES = 0x18,
            IRON_AR_BEGIN_SUBENTRY = 0x19,
            IRON_AR_END_SUBENTRY = 0x20,
        };

        std::ifstream iaFile;
        char* archive;
        int idx = 0;
        int length = 0;

        char Consume();
        char Peek();
        char Peek(int offset);
        bool ReachedEnd();
        bool WouldReachEnd(int offset);

        bool ExpectMagicNumber();
        bool ExpectSingular(MagicNumbers magic);
        Result<std::string> ReadString();
        Result<Archive*> ReadArchive(MagicNumbers start, MagicNumbers end);
        Result<std::vector<Archive::Item*>> ReadItems(Archive* owner, bool readStrings, MagicNumbers start, MagicNumbers end);
        Result<ArrayItem*> ReadArray(Archive* owner);
        Result<std::vector<char>> ReadBytes(int num);

        public:

        ArchiveReader(const char* path);
        Result<std::vector<Archive*>> LoadRawArchives();
        
    };

}