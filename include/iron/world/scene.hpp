#pragma once

#include "iron/world/object.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <vector>

#define SCENE_NO_PREFERENCE 0

namespace Iron {

    struct IronDLL ObjectWrapper {
        Object* object = nullptr;
        bool valid = false;
    };

    class IronDLL Scene {

        std::vector<ObjectWrapper> objects;

        void SetObjectId(int id, Object* object);
        void UnsetObjectId(int id);

        public:

        Scene(std::string path, bool loadSave);

        Result<Object*> GetObject(int id);
        Result<Object*> GetObject(std::string name);
        std::vector<Object*> GetObjectsByName(std::string name);
        std::vector<Object*> GetObjectsByComponent(std::string componentName);
        std::vector<Object*> GetObjectsByType(std::string typeName);
        std::vector<ObjectWrapper> GetObjects();

        int AssignObjectId(Object* object, int preference = SCENE_NO_PREFERENCE);
        int GetFirstUsableId();
        bool IsIdAvailable(int id);
        bool DoesIdExist(int id);

        Result<Object*> AddObject(std::string typeName, std::string entityName);
        void RemoveObject(ObjectId id);

        void Tick(float dt);
        void FixedTick();
        void Save();
        void Unload();

    };

}