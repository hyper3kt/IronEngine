#include "iron/world/object.hpp"
#include "iron/base/engine.hpp"

using namespace Iron;

void Object::DeclareObject(std::string typeName, Object* instance) {
    ObjectRegistry declaration = {typeName, instance};
    Engine::AddNewObject(declaration);
}

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

void Object::Init() {}
void Object::Tick(float dt) {}

Archive* Object::GetArchive() {
    return archive;
}

void Object::Orphan() {
    if(parent) {
        parent->Orphan(this);
    }
}

void Object::Orphan(Object* child) {
    for(int i = 0; i < children.size(); i++) {
        Object* obj = children.at(i);

        if(obj->GetName() == child->GetName()) {
            children.erase(children.begin() + i);
            archive->DetachArchive(child->GetArchive());
            break;
        }
    }
}

void Object::SetParent(Object* parent) {
    Orphan();
    this->parent = parent;
}

void Object::AddChild(Object* child) {
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

void Object::AddChildren(std::vector<Object*> children) {
    for(int i = 0; i < children.size(); i++) {
        AddChild(children.at(i));
    }
}

void Object::Delete() {
    Orphan();

    for(int i = 0; i < children.size(); i++) {
        Object* child = children.at(i);
        archive->DetachArchive(child->GetArchive());
        child->Delete();
    }

    archive->Clean();

    for(int i = 0; i < components.size(); i++) {
        // TODO (Haven't implemented components quite yet)
    }

    delete this;
}

void Object::DeleteChild(std::string name) {
    for(int i = 0; i < children.size(); i++) {
        Object* obj = children.at(i);

        if(obj->GetName() == name) {
            obj->Delete();
        }
    }
}

template<typename T>
bool Object::AttachComponent() {
    T* comp = new T();

    for(int i = 0; i < components.size(); i++) {
        Component* child = components.at(i);

        if(comp->GetComponentName() == child->GetComponentName()) {
            delete comp;
            return false;
        }
    }

    delete comp;
    components.push_back(comp);
    return true;
}

template<typename T>
bool Object::RemoveComponent() {
    T* comp = new T();

    for(int i = 0; i < components.size(); i++) {
        Component* child = components.at(i);

        if(comp->GetComponentName() == child->GetComponentName()) {
            delete child;
            delete comp;
            return true;
        }
    }

    delete comp;
    return false;
}

template<typename T>
Result<T*> Object::GetComponent() {
    T* comp = new T();

    for(int i = 0; i < components.size(); i++) {
        Component* child = components.at(i);

        if(comp->GetComponentName() == child->GetComponentName()) {
            delete comp;
            return child;
        }
    }

    delete comp;
    return Failure(IRON_RESULT_NONEXISTENT_REQUEST);
}

template<typename T>
bool Object::HasComponent() {
    T* comp = new T();

    for(int i = 0; i < components.size(); i++) {
        Component* child = components.at(i);

        if(comp->GetComponentName() == child->GetComponentName()) {
            delete comp;
            return true;
        }
    }

    delete comp;
    return false;
}