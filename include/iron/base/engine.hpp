#pragma once

#include "iron/result.hpp"
#include "iron/renderer/renderer.hpp"
#include "iron/base/window.hpp"
#include "iron/base/config.hpp"
#include "iron/world/scene.hpp"
#include "iron/macros.hpp"

#include <boost/unordered_map.hpp>

#include <iostream>
#include <vector>
#include <functional>

namespace Iron {

	struct IronDLL ObjectRelationship {
		std::string typeName;
		std::function<Object*()> creator;
	};

	struct IronDLL ComponentRelationship {
		std::string componentName;
		std::function<Component*()> creator;
	};

	class IronDLL Engine {

		static Renderer* renderer;
		static Scene* scene;
		static Config gameConfig;
		static Config settingsConfig;

		static std::vector<Window> windows;
		static int selectedWindow;

		static std::string gameName;

		static bool shouldKill;
		static bool loadedConfigs;
		static bool useVulkan;

		static boost::unordered_map<std::string, std::function<Object*()>> objRelations;
		static boost::unordered_map<std::string, std::function<Component*()>> compRelations;

		Engine();

		static void Kill();

		public:

		static void Init();
		static void Ignite();
		static void KillEngine();

		static Result<EngineResult> LoadConfigs(std::string game, std::string settings);
		
		static void AddObjectRelationship(ObjectRelationship or_);
		static Result<std::function<Object*()>> GetObjectRelationship(std::string name);
		static void AddCompRelationship(ComponentRelationship cr);
		static Result<std::function<Component*()>> GetComponentRelationship(std::string name);
		static Scene* GetScene();
		static Result<EngineResult> LoadScene(const char* path);
		static Result<EngineResult> LoadSceneFromState(const char* path, const char* savePath);
		static void SetGameName(std::string name);
		static std::string GetGameName();

		static bool ShouldUseVulkan();
		static bool ContinueRunning();

	};

}

#define DefineObjectRelationship(name, value) { struct ObjectRelationship or_; \
	or_.typeName = name; \
	or_.creator = []() { return new value(); }; \
	Engine::AddObjectRelationship(or_); }

#define DefineCompRelationship(name, value) { struct ComponentRelationship cr; \
	cr.typeName = name; \
	cr.creator = []() { return new value(); }; \
	Engine::AddCompRelationship(cr); }
