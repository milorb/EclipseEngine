#include "SceneDB.hpp"

luabridge::LuaRef SceneDB::Find(std::string name) {

    if (actor_name_to_ids.find(name) == 
        actor_name_to_ids.end()) {
        
        return luabridge::newTable(ComponentDB::__State);
    }

    auto ids = actor_name_to_ids[name];
    int min_id = *ids.begin();

    if (actor_name_to_ids.find(name) == 
        actor_name_to_ids.end() ||
        ids.empty() ||
        actors_to_destroy_ids.find(min_id) != 
        actors_to_destroy_ids.end()) {
        
        return luabridge::newTable(ComponentDB::__State);
    } 
    else if (actors_to_instantiate.find(min_id) 
             != actors_to_instantiate.end()) {
        
        return *actors_to_instantiate[min_id]->self_ref;
    }

    return *actors[min_id]->self_ref;
}

luabridge::LuaRef SceneDB::FindAll(std::string name) {

    auto ids = actor_name_to_ids[name];

    if (actor_name_to_ids.find(name) == 
        actor_name_to_ids.end() ||
        ids.empty()) {
        
        return luabridge::newTable(ComponentDB::__State);
    }

    luabridge::LuaRef table = luabridge::newTable(ComponentDB::__State);
    int idx = 1;

    //this should just fall through when all ids are to be destroyed
    for (auto& id : ids) {

        if (actors_to_destroy_ids.find(id)
            != actors_to_destroy_ids.end()) {
            continue;
        }
        if (actors_to_instantiate.find(id) 
            != actors_to_instantiate.end()) {
            table[idx] = *actors_to_instantiate[id]->self_ref;
        } else {
            table[idx] = *actors[id]->self_ref;
        }
        idx++;
    }
    return table;
}

/*
Creates a new actor based on the specified actor template and return a reference to it.
Note : The actor's components won’t begin executing until next frame,
but the actor should be discoverable via Actor.Find() / Actor.FindAll() immediately.
*/
luabridge::LuaRef SceneDB::Instantiate(std::string template_name) {

    if (TemplateDB::templates.find(template_name) == 
        TemplateDB::templates.end()) {
        
        TemplateDB::load_template(template_name);
    }

    next_actor_id++;
    actors_to_instantiate[next_actor_id] = std::make_shared<Actor>(next_actor_id);
    
    actors_to_instantiate[next_actor_id]->inherit_from_template(
        *TemplateDB::templates[template_name]);

    actor_name_to_ids[template_name].insert(next_actor_id);

    return *actors_to_instantiate[next_actor_id]->self_ref;
}

/*
Destroy an actor, removing it and all of its components from the scene.
No lifecycle functions on the actor’s components should run after this function call 
(i.e. set enabled to false on all the components).
*/
void SceneDB::Destroy(luabridge::LuaRef actor_ref) {

    int id = actor_ref["GetID"](actor_ref);

    if (actors_to_instantiate.find(id) != 
        actors_to_instantiate.end()) {
        
        auto& actor_to_destroy = actors_to_instantiate[id];
        actor_to_destroy->to_destroy = true;
        actors_to_destroy_ids.insert(id);
        actor_name_to_ids[actor_to_destroy->name].erase(id);
        return;
    }

    if (actors.find(id) != actors.end()) {
        
        auto& actor_to_destroy = actors[id];
        actor_to_destroy->to_destroy = true;
        
        for (auto& [key, component] : actor_to_destroy->components) {
            (*component.ref)["enabled"] = false;
        }

        actors_to_destroy_ids.insert(id);
        actor_name_to_ids[actor_to_destroy->name].erase(id);
        return;
    }
}

void SceneDB::init() {
    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginNamespace("Actor")
        .addFunction("Find",
            &SceneDB :: Find)
        .addFunction("FindAll",
            &SceneDB :: FindAll)
        .addFunction("Instantiate",
            &SceneDB :: Instantiate)
        .addFunction("Destroy",
            &SceneDB :: Destroy)
        .endNamespace();

    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginNamespace("Scene")
        .addFunction("Load", 
            &SceneDB :: Load)
        .addFunction("GetCurrent", 
            &SceneDB :: GetCurrent) 
        .addFunction("DontDestroy", 
            &SceneDB :: DontDestroy)
        .endNamespace();
}

void SceneDB::Load(std::string scene_name) {
    to_load = { true, scene_name };
}

std::string SceneDB::GetCurrent() {
    return active_scene_name;
}

void SceneDB::DontDestroy(luabridge::LuaRef actor_ref) {
    int id = actor_ref["GetID"](actor_ref);
    actors_to_persist.insert(id);
}

void SceneDB::switch_scene() {
    std::string scene_to_load = to_load.second;
    to_load = { false , "" };
    
    std::set<int> to_destroy;

    for (auto& [id, actor] : SceneDB::actors) {
        if (actors_to_persist.find(id) 
            != actors_to_persist.end()) {
            
            continue;
        }
        if (actors.find(id) != actors.end()) {
            to_destroy.insert(id);
            actors[id]->to_destroy = true;
        }
        
        actors_with_update.erase(id);
        actors_with_late_update.erase(id);
    }
    actor_name_to_ids.clear();
    
    // call OnDestroy() before removal
    for (auto id : to_destroy) {
        actors[id]->OnDestroy();
        actors.erase(id);
    }

    load_scene(scene_to_load);

    for (auto id : actors_to_persist) {
        std::string name = actors[id]->name;
        actor_name_to_ids[name].insert(id);
    }
}

void SceneDB::load_scene(std::string scene_name) {

    std::string search_path = "resources/scenes/" + scene_name + ".scene";
    active_scene_name = scene_name;

    //const rapidjson::Value& actors = scene_doc["actors"];
    if (!std::filesystem::exists(search_path)) {
        std::cout << "error: scene " << scene_name << " is missing";
        std::exit(0);
    }
    
    rapidjson::Document scene_doc;
    Utilities::read_json_file(search_path, scene_doc);
    const rapidjson::Value& actor_arr = scene_doc["actors"];
    
    for (auto& actor : actor_arr.GetArray()) {

        next_actor_id++;
        actors[next_actor_id] = std::make_shared<Actor>(Actor(next_actor_id));

        // check for templates, adding them to the database if they have yet to be loaded
        if (actor.HasMember("template")) {
            std::string template_name = actor["template"].GetString();
            if (TemplateDB::templates.find(template_name) == TemplateDB::templates.end()) {
                TemplateDB::load_template(template_name);
            }
            actors[next_actor_id]->id = next_actor_id;
            actors[next_actor_id]->inherit_from_template(
                *TemplateDB::templates[template_name]);
        } 
        
        std::shared_ptr<Actor> actor_new = actors[next_actor_id];
        
        if (actor.HasMember("name")) {
            actor_new->name = actor["name"].GetString();
        }
        if (actor.HasMember("components")) {
            if (!actor["components"].IsObject()) {
                std::cout << "error: components isn't object";
                std::exit(0);
            }
            ComponentDB::load_components(actor_new, actor["components"]);
        }

        if (actor_new->components.empty()) {
            //STILL NEEDS A REFERENCE TO SELF!!
            luabridge::LuaRef no_comp = luabridge::newTable(ComponentDB::__State);
            actor_new->inject_reference_to_self(std::make_shared<luabridge::LuaRef>(no_comp));
        }
        
        on_start_queue.push(actor_new);
        actor_name_to_ids[actor_new->name].insert(actor_new->id);
    }
}

void SceneDB::start() {
    while (!on_start_queue.empty()) {
        on_start_queue.front()->Start();
        on_start_queue.pop();
    }
}

void SceneDB::update() {
    for (const auto& [id, actor]  : actors) {
        actor->Update();
    }
}

void SceneDB::update_L() {
    for (const auto& [id, actor] : actors) {
        actor->LateUpdate();
    }
}


void SceneDB::destroy_acs() {
    for (int id : actors_to_destroy_ids) {
        std::string name = "";

        if (actors_to_instantiate.find(id) !=
            actors_to_instantiate.end()) {
            name = actors_to_instantiate[id]->name;
            actors_to_instantiate[id]->OnDestroy();
            actors_to_instantiate.erase(id);
        }
        else {
            name = actors[id]->name;
            actors[id]->OnDestroy();
            actors.erase(id);
        }
    }
    actors_to_destroy_ids.clear();
}

void SceneDB:: instantiate_acs() {
    for (const auto& [id, actor] :
        actors_to_instantiate) {
        actors.emplace(id, actor);
        on_start_queue.push(actor);
    }
    actors_to_instantiate.clear();
}

void SceneDB::add_components() {
    for (const auto& [id, actor] : actors) {
        if (!actor->components_to_add.empty()) {
            on_start_queue.push(actor);
        }
        actor->add_components();
    }
}

void SceneDB::remove_components() {
    for (const auto& [id, actor] : actors) {
        actor->remove_components();
    }
}
