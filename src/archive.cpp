#include "iron/base/archive.hpp"

using namespace Iron;

void Archive::SetName(std::string name) {
    this->name = name;
}

std::string Archive::GetName() {
    return name;
}

void Archive::SetType(std::string type) {
    this->type = type;
}

std::string Archive::GetType() {
    return type;
}

void Archive::AttachItem(Item* item) {
    items.push_back(item);
}

void Archive::AttachArchive(Archive* archive) {
    childArchives.push_back(archive);
}

void Archive::DetachArchive(std::string name) {
    for(int i = 0; i < childArchives.size(); i++) {
        Archive* child = childArchives.at(i);

        if(child->GetName() == name) {
            childArchives.erase(childArchives.begin() + i);
        }
    }
}

void Archive::Clean() {
    for(int i = 0; i < items.size(); i++) {
        delete items.at(i);
    }
}

ArrayItem* Archive::RetrieveArray(std::string name) {
    for(int i = 0; i < items.size(); i++) {
        Item* item = items.at(i);

        if(item->GetName() == name) {
            if(item->GetType() != IRON_ITEM_ARRAY) {
                break;
            }

            return static_cast<ArrayItem*>(item);
        }
    }

    ArrayItem* item = new ArrayItem(this);
    items.push_back(item);
    
    return item;
}

BoolItem* Archive::RetrieveBool(std::string name, bool def) {
    for(int i = 0; i < items.size(); i++) {
        Item* item = items.at(i);

        if(item->GetName() == name) {
            if(item->GetType() != IRON_ITEM_BOOL) {
                break;
            }

            return static_cast<BoolItem*>(item);
        }
    }

    BoolItem* item = new BoolItem(this, def);
    items.push_back(item);

    return item;
}

#define NumberImpl(type) NumberItem* Archive::RetrieveNumber(std::string name, type def) { \
    for(int i = 0; i < items.size(); i++) { \
        Item* item = items.at(i); \
        if(item->GetName() == name) { \
            if(item->GetType() != IRON_ITEM_NUMBER) break; \
            return static_cast<NumberItem*>(item); \
        } \
    } \
    NumberItem* item = new NumberItem(this, def); \
    items.push_back(item); \
    return item; \
}

NumberImpl(double)
NumberImpl(float)
NumberImpl(int)

StringItem* Archive::RetrieveString(std::string name, std::string def) {
    for(int i = 0; i < items.size(); i++) {
        Item* item = items.at(i);

        if(item->GetName() == name) {
            if(item->GetType() != IRON_ITEM_STRING) {
                break;
            }

            return static_cast<StringItem*>(item);
        }
    }

    StringItem* item = new StringItem(this, def);
    items.push_back(item);

    return item;
}

Item::Item() {
    owner = nullptr;
}

Item::Item(Archive* owner) {
    this->owner = owner;
    
    if(owner != nullptr) {
        owner->AttachItem(this);
    }
}

void Item::SetName(std::string name) {
    this->name = name;
}

std::string Item::GetName() {
    return name;
}

ItemTypeID Item::GetType() {
    return IRON_ITEM_GENERIC;
}

void Item::Pass(std::vector<char> bytes) {}

#define DefaultItemImpl(Type) Type::Type() {} Type::Type(Archive* owner) { \
    if(owner != nullptr) { \
        this->owner = owner; \
        owner->AttachItem(this); \
    } \
}

DefaultItemImpl(ArrayItem)
DefaultItemImpl(BoolItem)
DefaultItemImpl(NumberItem)
DefaultItemImpl(StringItem)

void ArrayItem::Append(Item* item) {
    items.push_back(item);
}

void ArrayItem::Remove(int idx) {
    if(idx >= Size()) {
        return;
    }

    items.erase(items.begin() + idx);
}

void ArrayItem::Pop() {
    items.pop_back();
}

int ArrayItem::Size() {
    return items.size();
}

Result<Item*> ArrayItem::At(int idx) {
    if(idx >= Size()) {
        return Failure(IRON_RESULT_TOO_BIG);
    }

    return items.at(idx);
}

BoolItem::BoolItem(Archive* owner, bool def) {
    if(owner != nullptr) {
        this->owner = owner;
        owner->AttachItem(this);
    }

    value = def;
}

void BoolItem::Pass(std::vector<char> bytes) {
    if(bytes.at(0)) {
        value = true;
        return;
    }

    value = false;
}

BoolItem& BoolItem::operator=(bool rhs) {
    value = rhs;
    return *this;
}

BoolItem::operator bool() {
    return value;
}

NumberItem::NumberItem(Archive* owner, double def) {
    if(owner != nullptr) {
        this->owner = owner;
        owner->AttachItem(this);
    }

    value = def;
}

void NumberItem::Pass(std::vector<char> bytes) {
    memcpy(&value, bytes.data(), sizeof(double));
}

NumberItem& NumberItem::operator=(double rhs) {
    value = rhs;
    return *this;
}

NumberItem::operator double() {
    return value;
}

StringItem::StringItem(Archive* owner, std::string def) {
    if(owner != nullptr) {
        this->owner = owner;
        owner->AttachItem(this);
    }

    value = def;
}

void StringItem::Pass(std::string bytes) {
    value = bytes;
}

StringItem& StringItem::operator=(std::string rhs) {
    value = rhs;
    return *this;
}

StringItem::operator std::string() {
    return value;
}

StringItem::operator const char*() {
    return value.c_str();
}