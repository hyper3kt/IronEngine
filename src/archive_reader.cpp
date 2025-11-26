#include "iron/base/archive_reader.hpp"
#include "iron/base/engine.hpp"

#include <sstream>
#include <iterator>

using namespace Iron;

ArchiveReader::ArchiveReader(const char* path) {
    iaFile = std::ifstream(path, std::ios::binary);
}

char ArchiveReader::Consume() {
    char c = archive[idx];

    if(idx < length) {
        idx++;
    }
    
    return c;
}

char ArchiveReader::Peek(int offset) {
    if(idx + offset < length) {
        return archive[idx + offset];
    }

    return archive[length - 1];
}

char ArchiveReader::Peek() {
    return Peek(0);
}

bool ArchiveReader::ReachedEnd() {
    return idx + 1 >= length;
}

bool ArchiveReader::WouldReachEnd(int offset) {
    return idx + offset >= length;
}

// Yes, I made the magic number BADDIES. I am very funny.
enum FileIdentity {
    ARCHIVE_BA = 0xBA,
    ARCHIVE_DD = 0xDD,
    ARCHIVE_IE = 0x13,
    ARCHIVE_SS = 0x55,
};

bool ArchiveReader::ExpectMagicNumber() {
    if(Consume() == ARCHIVE_BA && Consume() == ARCHIVE_DD && Consume() == ARCHIVE_IE && Consume() == ARCHIVE_SS) {
        return true;
    }

    return false;
}

bool ArchiveReader::ExpectSingular(MagicNumbers magic) {
    return Consume() == magic;
}

Result<std::string> ArchiveReader::ReadString() {
    std::string working = "";

    while(Consume()) {
        if(ReachedEnd()) {
            return Failure(IRON_RESULT_TOO_BIG);
        }

        char c = Peek();

        working += c;

        if(c == IRON_AR_END_STRING) {
            return working;
        }
    }
}

Result<std::vector<char>> ArchiveReader::ReadBytes(int num) {
    std::vector<char> out;

    for(int i = 0; i < num; i++) {
        if(ReachedEnd()) {
            return Failure(IRON_RESULT_TOO_BIG);
        }

        out.push_back(Consume());
    }

    return out;
}

Result<ArrayItem*> ArchiveReader::ReadArray(Archive* owner) {
    auto getItems = ReadItems(nullptr, false, IRON_AR_BEGIN_ARRAY, IRON_AR_END_ARRAY);

    if(!getItems.Success()) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    ArrayItem* out = new ArrayItem(owner);
    auto items = getItems.GetValue();

    for(int i = 0; i < items.size(); i++) {
        out->Append(items.at(i));
    }

    return out;
}

Result<std::vector<Archive::Item*>> ArchiveReader::ReadItems(Archive* owner, bool readStrings, MagicNumbers start, MagicNumbers end) {
    if(!ExpectSingular(start)) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    std::vector<Archive::Item*> out;

    while(!ExpectSingular(end)) {
        Archive::Item* item;
        auto name = Result<std::string>("");

        if(ReachedEnd()) {
            return Failure(IRON_RESULT_TOO_BIG);
        }

        if(readStrings) {
            name = ReadString();

            if(!name.Success()) {
                return Failure(IRON_RESULT_IMPROPER_FORMAT);
            }
        }

        switch(Consume()) {
            case Archive::ItemTypeID::IRON_ITEM_ARRAY:
            auto getArray = ReadArray(owner);

            if(!getArray.Success()) {
                return Failure(IRON_RESULT_IMPROPER_FORMAT);
            }

            item = getArray.GetValue();
            break;
            case Archive::ItemTypeID::IRON_ITEM_BOOL:
            auto getBool = ReadBytes(1);

            if(!getBool.Success()) {
                return Failure(IRON_RESULT_IMPROPER_FORMAT);
            }

            item = new BoolItem(owner);
            item->Pass(getBool.GetValue());
            break;
            case Archive::ItemTypeID::IRON_ITEM_NUMBER:
            auto getNumber = ReadBytes(8);

            if(!getNumber.Success()) {
                return Failure(IRON_RESULT_IMPROPER_FORMAT);
            }

            item = new NumberItem(owner);
            item->Pass(getNumber.GetValue());
            break;
            case Archive::ItemTypeID::IRON_ITEM_STRING:
            auto getString = ReadString();

            if(!getString.Success()) {
                return Failure(IRON_RESULT_IMPROPER_FORMAT);
            }

            item = new StringItem(owner);
            StringItem* ptr = static_cast<StringItem*>(item);
            
            if(!ptr) {
                return Failure(IRON_RESULT_FAILED);
            }

            ptr->Pass(getString.GetValue());
            break;
            default:
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        if(readStrings) {
            item->SetName(name.GetValue());
        }

        out.push_back(item);
    }

    return out;
}

Result<Archive*> ArchiveReader::ReadArchive(MagicNumbers start, MagicNumbers end) {
    if(!ExpectSingular(start)) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    Archive* raw = new Archive();

    auto getName = ReadString();

    if(!getName.Success()) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    raw->SetName(getName.GetValue());

    if(!ExpectSingular(IRON_AR_TYPE)) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    auto getType = ReadString();

    if(!getType.Success()) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    if(!Engine::HasObjectRegistered(getType.GetValue())) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    raw->SetType(getType.GetValue());

    auto getItems = ReadItems(raw, true, IRON_AR_BEGIN_ENTRIES, IRON_AR_END_ENTRIES);

    if(!getItems.Success()) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    char choice = Peek();

    if(choice != IRON_AR_BEGIN_SUBENTRIES) {
        if(choice != end) {
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        return raw;
    }

    while(!ExpectSingular(IRON_AR_END_SUBENTRIES)) {
        if(ReachedEnd()) {
            return Failure(IRON_RESULT_TOO_BIG);
        }
        
        auto getArchive = ReadArchive(IRON_AR_BEGIN_SUBENTRY, IRON_AR_END_SUBENTRY);

        if(!getArchive.Success()) {
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        raw->AttachArchive(getArchive.GetValue());
    }

    if(!ExpectSingular(end)) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    return raw;
}

Result<std::vector<Archive*>> ArchiveReader::LoadRawArchives() {
    if(iaFile.bad()) {
        return Failure(IRON_RESULT_FAILED);
    }

    iaFile.seekg(0, iaFile.end);
    length = iaFile.tellg();
    iaFile.seekg(0, iaFile.beg);

    archive = (char*) malloc(length * sizeof(char));
    iaFile.read(archive, length);
    iaFile.close();

    if(!ExpectMagicNumber()) {
        free(archive);
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    std::vector<Archive*> out;

    while(idx < length) {
        auto getArchive = ReadArchive(IRON_AR_BEGIN_ARCHIVE, IRON_AR_END_ARCHIVE);

        if(!getArchive.Success()) {
            free(archive);
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        out.push_back(getArchive.GetValue());
    }
    
    free(archive);

    return out;
}