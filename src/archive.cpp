#include "iron/base/archive.hpp"

using namespace Iron;

void Archive::SetName(std::string name) {
    this->name = name;
}

void Archive::SetType(std::string type) {
    this->type = type;
}



Archive::Item::Item() {
    owner = nullptr;
}

Archive::Item::Item(Archive* owner) {
    this->owner = owner;
    
    if(owner != nullptr) {
        owner->AttachItem(this);
    }
}