#include "iron/world/scene.hpp"
#include "iron/base/archive.hpp"
#include "iron/base/engine.hpp"

using namespace Iron;

Scene::Scene(std::string path, bool loadSave) {

}

void Scene::SetObjectId(int id, Object* object) {
    if(!DoesIdExist(id) || !IsIdAvailable(id)) {
        return;
    }

    objects.at(id).object = object;
    objects.at(id).valid = true;
    object->SetId(id);
}

void Scene::UnsetObjectId(int id) {
    if(!DoesIdExist(id)) {
        return;
    }

    objects.at(id) = ObjectWrapper();
}

Result<Object*> Scene::GetObject(int id) {
    if(id >= objects.size()) {
        return Failure(IRON_RESULT_TOO_BIG);
    }

    auto wrapper = objects.at(id);

    if(!wrapper.valid) {
        return Failure(IRON_RESULT_NONEXISTENT_REQUEST);
    }

    return wrapper.object;
}

Result<Object*> Scene::GetObject(std::string name) {
    for(int i = 0; i < objects.size(); i++) {
        auto wrapper = objects.at(i);

        if(!wrapper.valid) {
            continue;
        }

        if(wrapper.object->GetName() == name) {
            return wrapper.object;
        }
    }

    return Failure(IRON_RESULT_NONEXISTENT_REQUEST);
}

std::vector<ObjectWrapper> Scene::GetObjects() {
    return objects;
}

int Scene::AssignObjectId(Object* object, int preference) {
    if(preference == SCENE_NO_PREFERENCE) {
        int id = GetFirstUsableId();
        SetObjectId(id, object);
        object->SetId(id);

        return id;
    }

    if(!IsIdAvailable(preference)) {
        return AssignObjectId(object, SCENE_NO_PREFERENCE);
    }

    SetObjectId(preference, object);
    return preference;
}

int Scene::GetFirstUsableId() {
    for(int i = 0; i < objects.size(); i++) {
        if(!objects.at(i).valid) {
            return i;
        }
    }

    objects.push_back(ObjectWrapper());
    
    return objects.size() - 1;
}

bool Scene::IsIdAvailable(int id) {
    if(!DoesIdExist(id)) {
        return false;
    }

    auto wrapper = objects.at(id);
    return !wrapper.valid;
}

bool Scene::DoesIdExist(int id) {
    return id < objects.size() && id > 0;
}

Result<Object*> Scene::AddObject(std::string typeName, std::string entityName) {
    auto getCreator = Engine::GetObjectRelationship(typeName);

    if(!getCreator.Success()) {
        return Failure(IRON_RESULT_NONEXISTENT_TYPE);
    }

    auto creator = getCreator.Value();
    Object* object = creator();
    SetObjectId(GetFirstUsableId(), object);

    object->SetName(entityName);
    object->Init();

    return object;
}

void Scene::RemoveObject(ObjectId id) {
    GetObjectFromId(id, object);
    int objectId = object->GetId();

    UnsetObjectId(objectId);
    object->Delete();
}

void Scene::Tick(float dt) {
    for(int i = 0; i < objects.size(); i++) {
        auto wrapper = objects.at(i);

        if(!wrapper.valid) {
            continue;
        }

        Object* object = wrapper.object;

        if(object->IsActive()) {
            object->PreTick(dt);
        }
    }
}

void Scene::FixedTick() {
    for(int i = 0; i < objects.size(); i++) {
        auto wrapper = objects.at(i);

        if(!wrapper.valid) {
            continue;
        }

        Object* object = wrapper.object;

        if(object->IsActive()) {
            object->PreFixedTick();
        }
    }
}