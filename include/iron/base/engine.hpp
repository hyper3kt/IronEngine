#pragma once

#include <iostream>
#include <vector>

namespace Iron {

	class Window;

	class Engine {

		static std::vector<Window> windows;
		static std::string gameName;
		static bool shouldKill;

		public:
		static void Init(std::string gameName);
		static void Kill();

		static void SetGameName(std::string name);
		static std::string GetGameName();

	};

}