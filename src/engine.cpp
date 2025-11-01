#include "iron/base/engine.h"
#include "iron/base/window.h"

using namespace Iron;

std::string Engine::gameName;

void Engine::Init() {
    SetGameName("Iron Engine");
    Window::InitSystem();
}

void Iron::Engine::SetGameName(std::string name) {
    name = gameName;
}

std::string Engine::GetGameName() {
	return gameName;
}