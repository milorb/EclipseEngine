#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <filesystem>
#include <memory>

#include "lua.hpp"
#include "LuaBridge.h"
#include "PackageManager.hpp"

class Component {
public:
    explicit Component() {};
    
    luabridge::LuaRef load_ref(std::string &type, lua_State* lua_state);

    template<typename... Args>
    void execute_function(std::string& func_name, Args&&... args);
    void execute_function(std::string& func_name, luabridge::LuaRef& self);

    bool contains_function(std::string& func_name);

    template<typename T>
    void inject_variable(const std::string& name, T& value);


	std::shared_ptr<luabridge::LuaRef> ref;
	std::string type;

    bool has_start = false;
    bool has_update = false;
    bool has_late_update = false;

    bool has_on_destroy = false;

    bool has_enter_collision = false;
    bool has_exit_collision = false;

    bool has_enter_trigger = false;
    bool has_exit_trigger = false;
};

#endif
