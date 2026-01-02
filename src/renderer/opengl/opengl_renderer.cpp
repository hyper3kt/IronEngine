#include "opengl_renderer.hpp"
#include "iron/base/engine.hpp"
#include "iron/world/scene.hpp"

#include <glad/glad.h>

using namespace Iron;

OpenGLRenderer::OpenGLRenderer() {
    
}

void OpenGLRenderer::Tick() {
    auto objects = Engine::GetScene()->GetObjects();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(int i = 0; i < objects.size(); i++) {
        auto wrapper = objects.at(i);

        if(!wrapper.valid) {
            continue;
        }

        auto object = wrapper.object;

        if(!object->IsVisible()) {
            continue;
        }

        //
    }
}
