#include "iron/world/camera.hpp"

using namespace Iron;

void Camera::SetFOV(float fov) {
    *FOV = fov;
}

float Camera::GetFOV() {
    return *FOV;
}

void Camera::Init() {
    FOV = archive->RetrieveNumber("fov", 90.0f);
}