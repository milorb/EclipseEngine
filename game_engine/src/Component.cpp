#include "Component.hpp"

luabridge::LuaRef Component::load_ref(std::string& type, lua_State* lua_state) {
    
    std::string working_path = std::filesystem::current_path().string();
    std::string new_path = working_path + "/resources/component_types/" + type + ".lua";

    // PUT CALLS TO LOAD PACKAGE COMPONENTS HERE!
    if (!std::filesystem::exists(new_path)) {
        new_path = "";
        std::cout << "searching for package component" << std::endl;
        new_path = PackageManager::get_package_filepath(type);
        if (new_path == "") {
            std::cout << "error: failed to locate component " << type;
            std::exit(0);
        }
    }

    //check to make sure the file exists
    if (luaL_loadfile(lua_state, new_path.c_str()) != LUA_OK) {
        std::cout << "problem with lua file " << type;
        std::exit(0);
    }

    luabridge::getGlobalNamespace(lua_state);

    //sandboxes lua table for error checking
    if (luaL_dofile(lua_state, new_path.c_str()) != LUA_OK) {
        std::cout << "problem with lua file "  << type;
        std::exit(0);
    }

    luabridge::LuaRef component = luabridge::getGlobal(lua_state, type.c_str());

    ref = std::make_shared<luabridge::LuaRef>(component);
    this->type = type;

    std::string func = "OnStart";
    has_start = contains_function(func);

    func = "OnUpdate";
    has_update = contains_function(func);

    func = "OnLateUpdate";
    has_late_update = contains_function(func);

    func = "OnCollisionEnter";
    has_enter_collision = contains_function(func);

    func = "OnCollisionExit";
    has_exit_collision = contains_function(func);

    func = "OnTriggerEnter";
    has_enter_trigger = contains_function(func);

    func = "OnTriggerExit";
    has_exit_trigger = contains_function(func);

    func = "OnDestroy";
    has_on_destroy = contains_function(func);

    (*ref)["enabled"] = true;

    return component;
}

template<typename... Args>
void Component::execute_function(std::string& func_name, Args&&... args) {
    if (contains_function(func_name)) {
        (*ref)[func_name.c_str()](std::forward<Args>(args)...);
    }
}

void Component::execute_function(std::string& func_name, luabridge::LuaRef& self) {
    if (contains_function(func_name)) {
        (*ref)[func_name.c_str()](self);
    }
}

bool Component::contains_function(std::string& func_name) {
    luabridge::LuaRef thing = (*ref)[func_name.c_str()];
    if (thing.isFunction()) {
        return true;
    }
    return false;
}

template<typename T>
void Component::inject_variable(const std::string& name, T& value) {
    (*ref)[name.c_str()] = value;
}

template void Component::inject_variable<int>(const std::string& name, int&);
template void Component::inject_variable<bool>(const std::string& name, bool&);
template void Component::inject_variable<float>(const std::string& name, float&);
template void Component::inject_variable<std::string>(const std::string& name, std::string&);
