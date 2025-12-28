#include "iron/base/engine.hpp"
#include "iron/base/window.hpp"
#include "renderer/vulkan/vulkan_renderer.hpp"
#include "renderer/opengl/opengl_renderer.hpp"
#include "iron/base/config.hpp"
#include "iron/world/scene.hpp"

#include <SDL3/SDL.h>

using namespace Iron;

Scene* Engine::scene = new Scene();
Renderer* Engine::renderer;

Config Engine::gameConfig = Config();
Config Engine::settingsConfig = Config();

int Engine::selectedWindow = 0;

std::string Engine::gameName = "Iron Engine";
bool Engine::shouldKill = false;
bool Engine::loadedConfigs = false;
bool Engine::useVulkan = false;

std::vector<ObjectRelationship> Engine::objRelations;
std::vector<ComponentRelationship> Engine::compRelations;

bool Engine::ShouldUseVulkan() {
    return useVulkan;
}

Result<EngineResult> Engine::LoadConfigs(std::string game, std::string settingsPath) {
    auto loadGame = gameConfig.LoadConfig(game);
    loadedConfigs = true;
    
    if(!loadGame.Success()) {
        loadedConfigs = false;
        return Failure(IRON_RESULT_FAILED);
    }

    auto loadSettings = settingsConfig.LoadConfig(settingsPath);

    if(!loadSettings.Success()) {
        // TODO: defaults
    }

    return IRON_RESULT_CREATED;
}

Scene* Engine::GetScene() {
    return scene;
}

Result<EngineResult> Engine::LoadScene(const char* path) {
    /*if(!Scene::HasScene(path)) {
        return Failure(IRON_RESULT_NONEXISTENT_REQUEST);
    }

    auto loadScene = Scene::LoadScene(path);

    if(!loadScene.Success()) {
        return loadScene.Fail();
    }

    scene = loadScene.Value();
    scene->BareLoad();*/

    return IRON_RESULT_LOADED;
}

Result<EngineResult> Engine::LoadSceneFromState(const char* path, const char* savePath) {
    /*if(!Scene::HasScene(path)) {
        return Failure(IRON_RESULT_NONEXISTENT_REQUEST);
    }

    auto loadScene = Scene::LoadScene(path);

    if(!loadScene.Success()) {
        return loadScene.Fail();
    }

    scene = loadScene.Value();
    scene->FromState(savePath);*/

    return IRON_RESULT_LOADED;
}

void Engine::Init() {
    if(!loadedConfigs) {
        auto tryLoad = LoadConfigs("./game.ic", "./settings.ic");

        if(!tryLoad.Success()) {
            Kill();
            return;
        }
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

    auto sceneName = gameConfig.GetEntry("default-scene", false);

    if(!sceneName.Success()) {
        Kill();
        return;
    }

    auto loadScene = LoadScene(sceneName.Value()->String().c_str());

    if(!loadScene.Success()) {
        Kill();
        return;
    }
}

void Engine::Ignite() {
    DefineObjectRelationship("object", Object)
    DefineObjectRelationship("camera", Camera)

    while(!shouldKill) {

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

void Engine::AddObjectRelationship(ObjectRelationship or_) {
    objRelations.push_back(or_);
}

std::vector<ObjectRelationship> Engine::GetObjectRelationships() {
    return objRelations;
}

void Engine::AddCompRelationship(ComponentRelationship cr) {
    compRelations.push_back(cr);
}

std::vector<ComponentRelationship> Engine::GetCompRelationships() {
    return compRelations;
}