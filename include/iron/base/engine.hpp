#pragma once

#include "iron/result.hpp"
#include "iron/renderer/renderer.hpp"
#include "iron/base/window.hpp"
#include "iron/base/config.hpp"
#include "iron/world/scene.hpp"

#include <iostream>
#include <vector>

namespace Iron {

	class Engine {

		static Renderer* renderer;
		static Scene* scene;
		static Config gameConfig;
		static Config settingsConfig;
		static std::vector<ObjectRegistry> registry;

		static std::vector<Window> windows;
		static int selectedWindow;

		static std::string gameName;

		static bool shouldKill;
		static bool loadedConfigs;
		static bool useVulkan;

		Engine();

		public:
		static void Init();
		static Result<EngineResult> LoadConfigs(std::string game, std::string settings);
		static void Kill();
		
		static Scene* GetCurrentScene();

		static void SetGameName(std::string name);
		static std::string GetGameName();

		static void AddNewObject(ObjectRegistry reg);
		static std::vector<ObjectRegistry> GetObjectRegistry();
		static bool HasObjectRegistered(std::string name);

		static bool ShouldUseVulkan();

	};

}
