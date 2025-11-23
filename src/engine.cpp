#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"
#include "renderer/vulkan/vulkan_renderer.hpp"
#include "renderer/opengl/opengl_renderer.hpp"
#include "iron/world/scene.hpp"

#include <SDL3/SDL.h>

using namespace Iron;

Config Engine::config = Config("");
Config Engine::settings = Config("");
Scene* Engine::scene = new Scene();

int Engine::selectedWindow = 0;

std::string Engine::gameName = "Iron Engine";
bool Engine::shouldKill = false;
bool Engine::loadedConfigs = false;
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

    loadedConfigs = true;

    return IRON_RESULT_OKAY;
}

void Engine::Init() {
    if(!loadedConfigs) {
        auto attemptLoadConfig = LoadConfigs("./game.ic", "./settings.ic");

        if(!attemptLoadConfig.Success()) {
            Kill();
            return;
        }
    }
    
    if(config.HasEntry("name")) {
        std::string entry = config.GetEntry("name").GetValue().data.string;
        gameName = entry;
    }

    if(!config.HasEntry("default scene")) {
        Kill();
        return;
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

    auto attemptLoadDefaultScene = Scene::LoadScene(config.GetEntry("default scene").GetValue().data.string.c_str());

    if(!attemptLoadDefaultScene.Success()) {
        Kill();
        return;
    }

    // The first scene shouldn't explicitly load its state.
    scene = &attemptLoadDefaultScene.GetValue();

    while(!shouldKill) {
        scene->Tick();
        renderer->Tick();
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

void Engine::AddNewObject(ObjectRegistry reg) {
    registry.push_back(reg);
}

std::vector<ObjectRegistry> Engine::GetObjectRegistry() {
    return registry;
}

bool Engine::HasObjectRegistered(std::string name) {
    for(int i = 0; i < registry.size(); i++) {
        if(registry.at(i).typeName == name) {
            return true;
        }
    }

    return false;
}