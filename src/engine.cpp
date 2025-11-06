#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"
#include "iron/config.hpp"

using namespace Iron;

Config Engine::config;
std::string Engine::gameName = "Iron Engine";
bool Engine::shouldKill = false;

#define GetStrFromEntry(str) config.GetEntry(str).GetValue().data.string;

void Engine::Init(std::string gameConfig) {
    //SetGameName(gameName);

    config = Config(gameConfig);
    Result<ConfigLoadStatus> loadResult = config.Load();

    if(!loadResult.Success()) {
        Kill();
        return;
    }
    
    Window::InitSystem();

    WindowSystemMetadata wsm = WindowSystemMetadata();

    if(config.HasEntry("name")) {
        std::string entry = GetStrFromEntry("name");
        wsm.name = entry;
        gameName = entry;
    }

    if(config.HasEntry("version")) {
        wsm.version = GetStrFromEntry("version");
    }

    if(config.HasEntry("appid")) {
        wsm.appId = GetStrFromEntry("appid");
    }

    if(config.HasEntry("developer")) {
        wsm.developer = GetStrFromEntry("developer");
    }

    if(config.HasEntry("copyright")) {
        wsm.copyright = GetStrFromEntry("copyright");
    }

    Window::SetWindowMetadata(wsm);

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