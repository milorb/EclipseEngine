#ifndef ComponentDB_hpp
#define ComponentDB_hpp

#include <iostream>
#include <filesystem>
#include <thread>
#include <map>

#include "rapidjson/document.h"
#include "Actor.hpp"
#include "Helper.h"
#include "Input.hpp"
#include "Physics.hpp"
#include "RigidBody.hpp"

class Physics;

class ComponentDB
{
public:
	static void init();
    static void create_child(const std::string& key, Component& child, const Component& parent);
    static void load_components(std::shared_ptr<Actor> actor_new, const rapidjson::Value& components);
    static void load_tiled_properties(std::shared_ptr<Actor> actor_new, const std::vector<std::string> properties,
                                      glm::vec2& pos, int gid);
    static void establish_inheritance(luabridge::LuaRef& instance, luabridge::LuaRef& parent);
    static void create_fixture(b2Body* body, std::shared_ptr<Actor> actor_new, RigidBody& rb);
    //static b2BodyDef load_body_def(const rapidjson::Value& rb);
    //void set_component();
    static bool component_type_exists(std::string& type);
    
    static void Log(const std::string& message);
    static void LogError(const std::string& error_message);

    static int GetFrame();
    static void Quit();
    static void Sleep(int milliseconds);
    static void OpenURL(std::string url);
    
    static inline int runtime_components_added = 0;
	static inline std::map<std::string, Component> components;
	static lua_State* __State;
};

#endif
