#include "iron/world/object.hpp"
#include "iron/base/engine.hpp"

using namespace Iron;

Result<Object*> ObjectId::GetObject() {
    return object;
}

ObjectId::ObjectId(int fromId) {
    object = Engine::GetScene()->GetObject(fromId);
}

ObjectId::ObjectId(std::string fromName) {
    object = Engine::GetScene()->GetObject(fromName);
}

ObjectId::ObjectId(Object* fromPtr) {
    if(fromPtr != nullptr) {
        object = fromPtr;
    }
}

Object::Object() {
    active = archive->RetrieveBool("active", true);
    visible = archive->RetrieveBool("visible", true);
}

Object::Object(Archive* archive) {
    this->archive = archive;
    active = archive->RetrieveBool("active", true);
    visible = archive->RetrieveBool("visible", true);
}

Object::Object(ObjectId id) {
    active = archive->RetrieveBool("active", true);
    visible = archive->RetrieveBool("visible", true);
    GetObjectFromId(id, parent);

    SetParent(parent);
}

Object::Object(ObjectId id, Archive* archive) {
    active = archive->RetrieveBool("active", true);
    visible = archive->RetrieveBool("visible", true);
    this->archive = archive;
    
    GetObjectFromId(id, parent);
    SetParent(parent);
}

std::string Object::GetName() {
    return archive->GetName();
}

void Object::SetName(std::string name) {
    archive->SetName(name);
}

int Object::GetId() {
    return id;
}

// note that this function only updates what an object thinks its id is.
void Object::SetId(int id) {
    this->id = id;
}

bool Object::IsActive() {
    return *active;
}

void Object::SetActive(bool active) {
    *(this->active) = active;
}

bool Object::IsVisible() {
    return *visible;
}

void Object::SetVisible(bool visibility) {
    *(this->visible) = visibility;
}

Result<Object*> Object::GetParent() {
    if(parent == nullptr) {
        return Failure(IRON_RESULT_UNINITIALIZED);
    }

    return parent;
}

void Object::SetParent(ObjectId id) {
    Orphan();
    this->parent = parent;
}

void Object::AddChild(ObjectId id) {
    GetObjectFromId(id, child);

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
    GetObjectFromId(id, obj);

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
        parent->OrphanChild(this);
    }
}

void Object::DeleteChild(ObjectId id) {
    GetObjectFromId(id, child);

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
        Engine::GetScene()->RemoveObject(child->GetId());
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

void Object::PreInit() {
    for(int i = 0; i < children.size(); i++) {
        children.at(i)->PreInit();
    }

    Init();
}

void Object::PreTick(float dt) {
    for(int i = 0; i < children.size(); i++) {
        children.at(i)->PreTick(dt);
    }

    Tick(dt);
}

void Object::PreFixedTick() {
    for(int i = 0; i < children.size(); i++) {
        children.at(i)->PreFixedTick();
    }

    FixedTick();
}