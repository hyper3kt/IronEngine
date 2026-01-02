#pragma once

#include "iron/base/component.hpp"
#include "iron/base/archive.hpp"
#include "iron/result.hpp"
#include "iron/macros.hpp"

#include <iostream>
#include <vector>

namespace Iron {

    class Object;

    class IronDLL ObjectId {

        Result<Object*> object = Failure(IRON_RESULT_UNINITIALIZED);

        public:

        ObjectId(std::string fromName);
        ObjectId(Object* fromPtr);
        ObjectId(int id);

        Result<Object*> GetObject();

    };

    class IronDLL Object {

        std::vector<Component*> components;
        int id = 0;
        BoolItem* active;
        BoolItem* visible;

        protected:

        Archive* archive = new Archive();
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
        int GetId();
        void SetId(int id);
        bool IsActive();
        void SetActive(bool active);
        bool IsVisible();
        void SetVisible(bool visibility);
        Result<Object*> GetParent();
        void SetParent(ObjectId id);
        void AddChild(ObjectId id);
        void AddChildren(std::vector<ObjectId> children);
        void Orphan();
        void OrphanChild(ObjectId id);
        void Delete();
        void DeleteChild(ObjectId id);
        Archive* GetArchive();

        void PreInit();
        virtual void Init();
        void PreTick(float dt);
        virtual void Tick(float dt);
        void PreFixedTick();
        virtual void FixedTick();
        
    };

}

#define GetObjectFromId(objId, result) Object* result = nullptr; { auto getObj = objId.GetObject(); if(!getObj.Success()) return; result = getObj.Value(); }
#define GetObjectFromIdWithError(objId, result, errorCode) Object* result = nullptr; \
    { \
        auto getObj = objId.GetObject(); \
        if(!getObj.Success()) return Failure(errorCode); \
        result = getObj.Value(); \
    }