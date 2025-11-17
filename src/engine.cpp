#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"
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

bool Engine::ShouldUseVulkan() {
    return useVulkan;
}

Config Engine::GetGameConfig() {
    return config;
}

Result<EngineResult> Engine::LoadConfigs(std::string game, std::string settingsPath) {
    config = Config(game);
    Result<EngineResult> loadResult = config.Load();
    
    if(!loadResult.Success()) {
        return loadResult;
    }

    settings = Config(settingsPath);
    loadResult = settings.Load();

    if(!loadResult.Success()) {
        settings = Config::CreateDefaultSettings();
    }

    return IRON_RESULT_OKAY;
}

void Engine::Init(std::string gameConfig, std::string settingsPath) {
    auto attemptLoadConfig = LoadConfigs(gameConfig, settingsPath);

    if(!attemptLoadConfig.Success()) {
        Kill();
        return;
    }
    
    if(config.HasEntry("name")) {
        std::string entry = config.GetEntry("name").GetValue().data.string;
        gameName = entry;
    }

    Window::InitSystem();

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
    delete renderer;
}

void Engine::SetGameName(std::string name) {
    name = gameName;
}

std::string Engine::GetGameName() {
	return gameName;
}
