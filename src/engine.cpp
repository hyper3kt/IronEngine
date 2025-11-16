#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"
#include "iron/config.hpp"
#include "renderer/vulkan/vulkan_renderer.hpp"
#include "renderer/opengl/opengl_renderer.hpp"
#include "iron/assets/asset_manager.hpp"
#include "iron/base/input_manager.hpp"

#include <SDL3/SDL.h>

using namespace Iron;

Config Engine::config = Config("");
Config Engine::settings = Config("");

int Engine::selectedWindow = 0;

std::string Engine::gameName = "Iron Engine";
bool Engine::shouldKill = false;
bool Engine::useVulkan = false;

#define GetStrFromEntry(str) config.GetEntry(str).GetValue().data.string;

bool Engine::ShouldUseVulkan() {
    return useVulkan;
}

void Engine::Init(std::string gameConfig, std::string settingsPath) {
    config = Config(gameConfig);
    Result<EngineResult> loadResult = config.Load();

    if(!loadResult.Success()) {
        Kill();
        return;
    }

    settings = Config(settingsPath);
    loadResult = settings.Load();

    if(!loadResult.Success()) {
        settings = Config::CreateDefaultSettings();
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

    useVulkan = Window::AttemptLoadVulkan();

    if(!useVulkan) {
        if(!Window::AttemptLoadOpenGL()) {
            Kill();
            return;
        }
        renderer = new OpenGLRenderer();
    } else renderer = new VulkanRenderer();

    while(!shouldKill) {
        inputManager.PollEvents();
    }

    Kill();
}

void Engine::Kill() {
    Window::KillSystem();
}

void Engine::SetGameName(std::string name) {
    name = gameName;
}

std::string Engine::GetGameName() {
	return gameName;
}
