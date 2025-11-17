#pragma once

#include "iron/config.hpp"
#include "iron/result.hpp"

#include <iostream>
#include <vector>

namespace Iron {

	class Window;
	class Renderer;
	class AssetManager;
	class InputManager;

	class Engine {

		static Renderer* renderer;
		static AssetManager assetManager;
		static InputManager inputManager;

		static std::vector<Window> windows;
		static int selectedWindow;

		static std::string gameName;
		static Config config;
		static Config settings;

		static bool shouldKill;
		static bool loadedConfigs;
		static bool useVulkan;

		Engine();

		public:
		static void Init();
		static Result<EngineResult> LoadConfigs(std::string game, std::string settings);
		static void Kill();

		static void SetGameName(std::string name);
		static std::string GetGameName();
		static Config GetGameConfig();

		static bool ShouldUseVulkan();
	};

}