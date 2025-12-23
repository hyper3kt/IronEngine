#pragma once

#include "iron/base/component.hpp"
#include "iron/base/archive.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <vector>

namespace Iron {

    class Object;

    class ObjectId {

        Result<Object*> object = Failure(IRON_RESULT_UNINITIALIZED);

        public:

        ObjectId(std::string fromName);
        ObjectId(Object* fromPtr);
        ObjectId(int id);

        Result<Object*> GetObject();

    };

    class Object {

        Archive* archive = nullptr;
        std::vector<Component*> components;
        int id = 0;

        protected:

        std::vector<Object*> children;
        Object* parent = nullptr;

        public:

        Object();
        Object(Archive* archive);
        Object(ObjectId parent);
        Object(ObjectId parent, Archive* archive);

        void AttachComponent(std::string name);
        void RemoveComponent(std::string name);
        Component* GetComponent(std::string name);
        bool HasComponent(std::string name);

        std::string GetName();
        void SetName(std::string name);
        void SetParent(ObjectId id);
        void AddChild(ObjectId id);
        void AddChildren(std::vector<ObjectId> children);
        void Orphan();
        void OrphanChild(ObjectId id);
        void Delete();
        void DeleteChild(ObjectId id);
        Archive* GetArchive();

        virtual void Init();
        virtual void Tick(float dt);
        virtual void FixedTick();

        // TODO: refactor
        static void DeclareObject(std::string typeName, Object* instance);
    };

    struct ObjectRegistry {
        std::string typeName;
        Object* instance;
    };

}

#define DerefObjectId(objId, result) auto getObj = objId.Object(); if(!getObj.Success()) return; auto result = getObj.Value()
