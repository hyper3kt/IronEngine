#include "iron/world/object.hpp"
#include "iron/base/engine.hpp"

using namespace Iron;

Object::Object() {
    archive = new Archive();
}

Object::Object(Archive* archive) {
    this->archive = archive;
}

std::string Object::GetName() {
    return archive->GetName();
}

void Object::SetName(std::string name) {
    archive->SetName(name);
}

void Object::SetParent(ObjectId id) {
    Orphan();
    this->parent = parent;
}

void Object::AddChild(ObjectId id) {
    DerefObjectId(id, child);

    for(int i = 0; children.size(); i++) {
        Object* obj = children.at(i);

        if(obj->GetName() == child->GetName()) {
            return;
        }
    }

    child->SetParent(this);
    archive->AttachArchive(child->GetArchive());
    children.push_back(child);
}

void Object::AddChildren(std::vector<ObjectId> children) {
    for(int i = 0; i < children.size(); i++) {
        AddChild(children.at(i));
    }
}

void Object::OrphanChild(ObjectId id) {
    DerefObjectId(id, obj);

    for(int i = 0; i < children.size(); i++) {
        Object* child = children.at(i);

        if(child->GetName() == obj->GetName()) {
            children.erase(children.begin() + i);
            archive->DetachArchive(child->GetName());
            break;
        }
    }
}

void Object::Orphan() {
    if(parent) {
        parent->OrphanChild(GetName());
    }
}

void Object::DeleteChild(ObjectId id) {
    DerefObjectId(id, child);

    for(int i = 0; i < children.size(); i++) {
        Object* obj = children.at(i);

        if(obj->GetName() == child->GetName()) {
            obj->Delete();
        }
    }
}

void Object::Delete() {
    Orphan();

    for(int i = 0; i < children.size(); i++) {
        Object* child = children.at(i);
        archive->DetachArchive(child->GetName());
        child->Delete();
    }

    archive->Clean();

    for(int i = 0; i < components.size(); i++) {
        // TODO (Haven't implemented components quite yet)
    }

    delete this;
}

Archive* Object::GetArchive() {
    return archive;
}

void Object::Init() {}
void Object::Tick(float dt) {}
void Object::FixedTick() {}