#pragma once

#include <iostream>
#include <vector>

namespace Iron {

	class Window;
	class Config;

	class Engine {

		static std::vector<Window> windows;
		static std::string gameName;
		static Config config;
		static Config settings;
		static bool shouldKill;

		public:
		static void Init(std::string gameConfig, std::string settingsPath);
		static void Kill();

		static void SetGameName(std::string name);
		static std::string GetGameName();

	};

}