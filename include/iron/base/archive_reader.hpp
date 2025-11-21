#pragma once

#include "iron/base/archive.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <fstream>

namespace Iron {

    class ArchiveReader {

        enum MagicNumbers {
            IRON_AR_BEGIN_ARCHIVE = 0x00,
            IRON_AR_END_ARCHIVE = 0X01,
        };

        std::ifstream iaFile;
        char* archive;
        int idx = 0;
        int length = 0;

        char Consume();

        bool ExpectMagicNumber();
        bool ExpectSingular(MagicNumbers magic);

        public:

        ArchiveReader(const char* path);
        Result<Archive> LoadRawArchives();
    };

}