#include "iron/world/object.hpp"
#include "iron/base/engine.hpp"

using namespace Iron;

void Object::DeclareObject(std::string typeName, Object* instance) {
    ObjectRegistry declaration = {typeName, instance};
    Engine::AddNewObject(declaration);
}