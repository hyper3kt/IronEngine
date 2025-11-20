#include "opengl_renderer.hpp"
#include "iron/base/engine.hpp"
#include "iron/world/scene.hpp"
#include "iron/renderer/render_component.hpp"

#include <glad/glad.h>

using namespace Iron;

OpenGLRenderer::OpenGLRenderer() {
    
}

void OpenGLRenderer::Tick() {
    Scene* scene = Engine::GetCurrentScene();
    std::vector<Object*> objects = scene->GetObjectsByComponent<RenderComponent>();

    for(int i = 0; i < objects.size(); i++) {
        Object* object = objects.at(i);
        auto result = object->GetComponent<RenderComponent>();
        
        if(!result.Success()) {
            continue;
        }

        RenderComponent* comp = result.GetValue();
    }

}