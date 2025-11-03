#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"

using namespace Iron;

std::string Engine::gameName;
bool Engine::shouldKill = false;

void Engine::Init(std::string gameName) {
    SetGameName(gameName);
    Window::InitSystem();

    while(!shouldKill) {

        // TODO

    }

    Window::KillSystem();
}

void Engine::Kill() {
    shouldKill = true;
}

void Engine::SetGameName(std::string name) {
    name = gameName;
}

std::string Engine::GetGameName() {
	return gameName;
}