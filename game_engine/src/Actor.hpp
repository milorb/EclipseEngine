#ifndef Actor_hpp
#define Actor_hpp

#include <string>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>

#include "glm/glm.hpp"
#include "Component.hpp"

class Listener;
struct Collision;
class ComponentDB;

class Actor {
public:
    explicit Actor(int id) : id(id) {};
    
    bool compare_id(const std::shared_ptr<Actor> a) const{
        return id < a->id;
    }

    std::string GetName();
    int GetID();

    luabridge::LuaRef GetComponentByKey(std::string key);

    luabridge::LuaRef GetComponent(std::string type_name);

    luabridge::LuaRef GetComponents(std::string type_name);

    luabridge::LuaRef AddComponent(std::string type_name);

    void RemoveComponent(luabridge::LuaRef comp_ref);

    void inherit_from_template(const Actor& actor_template);

    void inject_reference_to_self(std::shared_ptr<luabridge::LuaRef> component);

    //places the owned component into all relevant data structures
    //also calls inject_reference_to_self, "signing" the component
    void attach_component(Component& owned_component, const std::string& key);

    // this version removes all references from data structures
    void remove_components();

    void add_components();

    void Start();

    void Update();

    void LateUpdate();

    void OnDestroy();

    void EnterCollision(Collision& collision_info);

    void ExitCollision(Collision& collision_info);

    void EnterTrigger(Collision& collision_info);

    void ExitTrigger(Collision& collision_info);

    bool to_destroy = false;

    std::map <std::string, Component> components;

    //components waiting to be attached at frame end
    std::queue <std::shared_ptr<Component>> components_to_add;

    std::set <std::string> keys_to_remove;
    std::unordered_map<std::string, std::list<std::string>> component_type_to_key;

    std::set<std::string> on_start_keys;
    std::set<std::string> on_update_keys;
    std::set<std::string> on_late_update_keys;

    std::set<std::string> on_collision_enter_keys;
    std::set<std::string> on_collision_exit_keys;

    std::set<std::string> on_trigger_enter_keys;
    std::set<std::string> on_trigger_exit_keys;

    std::shared_ptr<luabridge::LuaRef> self_ref;
    std::string name;

    float id;
};

#endif /* Actor_hpp */
