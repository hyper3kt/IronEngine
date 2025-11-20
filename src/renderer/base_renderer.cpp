#include "iron/renderer/renderer.hpp"

using namespace Iron;

void Renderer::SetCamera(Camera* camera) {
    this->camera = camera;
}

Camera* Renderer::GetCamera() {
    return camera;
}