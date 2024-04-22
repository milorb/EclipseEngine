#ifndef SceneDB_hpp
#define SceneDB_hpp

#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include <queue>
#include <list>

#include "rapidjson/document.h"
#include "TemplateDB.hpp"
#include "ComponentDB.hpp"


class SceneDB {
public:
    static void init();
    void load_scene(std::string scene_name);
    void switch_scene();
    
    static void start();
    static void update();
    static void update_L();
    static void destroy_acs();
    static void  instantiate_acs();
    static void add_components();
    static void remove_components();

    static inline luabridge::LuaRef Find(std::string name);
    static inline luabridge::LuaRef FindAll(std::string name);
    static inline luabridge::LuaRef Instantiate(std::string template_name);
    static inline void Destroy(luabridge::LuaRef actor_ref);

    static inline void Load(std::string scene_name);
    static inline std::string GetCurrent();
    static inline void DontDestroy(luabridge::LuaRef actor);

    static inline std::unordered_map<std::string, std::set<int>> actor_name_to_ids;
    static inline std::map<int, std::shared_ptr<Actor>> actors; // stored by id

    static inline std::unordered_map<int, std::shared_ptr<Actor>> actors_to_instantiate;
    static inline std::queue<std::shared_ptr<Actor>> on_start_queue;

    static inline std::set<int> actors_to_destroy_ids;

    static inline std::set<int> actors_with_update;
    static inline std::set<int> actors_with_late_update;

    static inline std::set<int> actors_to_persist;

    static inline std::pair<bool, std::string> to_load = { false, "" };

    static inline std::string active_scene_name = "";
    static inline int next_actor_id = 0;
};
#endif /* SceneDB_hpp */
