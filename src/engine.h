#pragma once

#include "window.h"

#include <iostream>
#include <vector>

namespace Iron {

	class Engine {

		static std::vector<Window> windows;
		static std::string gameName;

		public:
		static void Init();

		static void SetGameName(std::string name);
		static std::string GetGameName();

	};

}