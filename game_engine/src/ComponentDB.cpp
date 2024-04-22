#include "ComponentDB.hpp"

lua_State* ComponentDB::__State = luaL_newstate();

void ComponentDB::init() {
    luaL_openlibs(__State);
    
    luabridge::getGlobalNamespace(__State)
        .beginNamespace("Debug")
        .addFunction("Log",
                ComponentDB::Log)
        .addFunction("LogError",
                ComponentDB::LogError)
        .endNamespace();
    
    luabridge::getGlobalNamespace(__State)
        .beginNamespace("Application")
        .addFunction("GetFrame",
                ComponentDB::GetFrame)
        .addFunction("Quit",
                ComponentDB::Quit)
        .addFunction("Sleep",
                ComponentDB::Sleep)
        .addFunction("OpenURL",
                ComponentDB::OpenURL)
        .endNamespace();
    
    luabridge::getGlobalNamespace(__State)
        .beginClass<Actor>("Actor")
        .addFunction("GetName",
                &Actor::GetName)
        .addFunction("GetID",
                &Actor::GetID)
        .addFunction("GetComponentByKey",
                &Actor::GetComponentByKey)
        .addFunction("GetComponent",
                &Actor::GetComponent)
        .addFunction("GetComponents",
                &Actor::GetComponents)
        .addFunction("AddComponent",
                &Actor::AddComponent)
        .addFunction("RemoveComponent",
                &Actor::RemoveComponent)
        .endClass();
    
    luabridge::getGlobalNamespace(__State)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", 
                &glm::vec2::x)
        .addProperty("y", 
                &glm::vec2::y)
        .endClass();
    
    luabridge::getGlobalNamespace(__State)
        .beginNamespace("Input")
        .addFunction("GetKey",
                Input::GetKey)
        .addFunction("GetKeyDown",
                Input::GetKeyDown)
        .addFunction("GetKeyUp",
                Input::GetKeyUp)
        .addFunction("GetMousePosition",
                Input::GetMousePosition)
        .addFunction("GetMouseButton",
                Input::GetMouseButton)
        .addFunction("GetMouseButtonDown",
                Input::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp",
                Input::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta",
                Input::GetMouseScrollDelta)
        .endNamespace();
}

void ComponentDB::create_child(const std::string& key, Component& child, const Component& parent) {

    child.type = parent.type;

    //make an instance of the component reference
    luabridge::LuaRef component_instance = luabridge::newTable(__State);
    establish_inheritance(component_instance, *parent.ref);
    child.ref = std::make_shared<luabridge::LuaRef>(component_instance);

    std::string key_to_inject = key;
    child.inject_variable("key", key_to_inject);

    child.has_start = parent.has_start;
    child.has_update = parent.has_update;
    child.has_on_destroy = parent.has_on_destroy;
    child.has_late_update = parent.has_late_update;
    child.has_enter_collision = parent.has_enter_collision;
    child.has_exit_collision = parent.has_exit_collision;
    child.has_enter_trigger = parent.has_enter_trigger;
    child.has_exit_trigger = parent.has_exit_trigger;
}

void ComponentDB::create_fixture(b2Body* body, std::shared_ptr<Actor> actor_new, RigidBody& rb) {

    if (!rb.has_collider && !rb.has_trigger) {
        b2PolygonShape phantom;
        phantom.SetAsBox(rb.width * 0.5f, rb.height * 0.5f);

        b2Filter filter;
        filter.categoryBits = 0x1000;

        b2FixtureDef phantom_def;
        phantom_def.shape = &phantom;

        phantom_def.filter = filter;
        phantom_def.isSensor = true;
        phantom_def.density = rb.density;
        phantom_def.userData.pointer = reinterpret_cast<uintptr_t> (&*actor_new);

        body->CreateFixture(&phantom_def);
    }
    
    if (rb.has_collider) {
        b2FixtureDef fix;

        b2Filter filter;
        filter.categoryBits = 0x0002;
        filter.maskBits = 0x0002;

        b2PolygonShape box;
        box.SetAsBox(rb.width * 0.5f, rb.height * 0.5f);

        b2CircleShape circle;
        circle.m_radius = rb.radius;


        if (rb.collider_type == "circle") {

            fix.shape = &circle;
        }
        else {

            fix.shape = &box;
        }

        fix.filter = filter;
        fix.isSensor = false;
        fix.density = rb.density;
        fix.friction = rb.friction;
        fix.restitution = rb.bounciness;
        fix.userData.pointer = reinterpret_cast<uintptr_t> (&*actor_new);

        body->CreateFixture(&fix);
    }

    if (rb.has_trigger) {
        b2FixtureDef fix;

        b2Filter filter;
        filter.categoryBits = 0x0070;
        filter.maskBits = 0x0070;

        b2PolygonShape box;
        box.SetAsBox(rb.trigger_width * 0.5f, rb.trigger_height * 0.5f);

        b2CircleShape circle;
        circle.m_radius = rb.trigger_radius;

        if (rb.trigger_type == "circle") {

            fix.shape = &circle;
        }
        else {

            fix.shape = &box;
        }

        fix.filter = filter;
        fix.isSensor = true;
        fix.density = rb.density;
        fix.userData.pointer = reinterpret_cast<uintptr_t> (&*actor_new);

        body->CreateFixture(&fix);
    }
}

void ComponentDB::load_components(std::shared_ptr<Actor> actor_new, const rapidjson::Value& actor_cmps) {
    
    // first loop through every component listed for the actor
    for (auto it = actor_cmps.MemberBegin(); it != actor_cmps.MemberEnd(); ++it) {

        std::string key = it->name.GetString();
        const rapidjson::Value& component = it->value;

        //if the component has a type inherit from ref of type, create a new ref if necessary  
        if (component.HasMember("type")) {
            
            std::string type = component["type"].GetString();
            
            if (type == "Rigidbody") {
                if (!Physics::world_created) {
                    Physics::initialize();
                    Physics::world_created = true;
                }

                Component comp_new;
                RigidBody rb;


                b2BodyDef body_def = rb.initialize(component);

                comp_new.type = type;

                b2Body* body = Physics::__World->CreateBody(&body_def);

                // now do stuff with the fixture
                create_fixture(body, actor_new, rb);

                rb.body = body;

                //push rigidbody onto lua stack, then retrieve a reference
                luabridge::push(__State, rb);
                luabridge::LuaRef ref = luabridge::LuaRef::fromStack(__State, -1);
                comp_new.ref = std::make_shared<luabridge::LuaRef>(ref);

                
                actor_new->components.insert(std::make_pair(key, comp_new));
                actor_new->component_type_to_key[type].push_back(key);

                continue;
            }
            
            //if there's no component of the type yet, load one
            else if (!component_type_exists(type)) {
                Component comp_new;
                comp_new.load_ref(type, __State);
                components[type] = comp_new;
            }

            //create a new component instance that will be owned by an actor
            Component actor_owned_component;
            create_child(key, actor_owned_component, components[type]);

            actor_new->attach_component(actor_owned_component, key);

        }

        for (auto var = component.MemberBegin(); var != component.MemberEnd(); ++var) {
            Component& comp_to_override = actor_new->components[key];
            std::string name = var->name.GetString();
            if (name != "type") {
                if (var->value.IsString()) {
                    std::string val = var->value.GetString();
                    comp_to_override.inject_variable(name, val);
                }
                if (var->value.IsInt()) {
                    int val = var->value.GetInt();
                    comp_to_override.inject_variable(name, val);
                }
                if (var->value.IsFloat()) {
                    float val = var->value.GetFloat();
                    comp_to_override.inject_variable(name, val);
                }
                if (var->value.IsBool()) {
                    bool val = var->value.GetBool();
                    comp_to_override.inject_variable(name, val);
                }
            }
        }
    }
}

void ComponentDB::load_tiled_properties(std::shared_ptr<Actor> actor_new,
                                        const std::vector<std::string> properties, glm::vec2& pos, int gid) {
    // first loop through every component listed for the actor
    for (int i = 0; i < properties.size(); ++i) {

        std::string key = std::to_string(i+1);
        std::string component_type = properties[i];

        //if the component has a type inherit from ref of type, create a new ref if necessary
            
        if (component_type == "Rigidbody") {
            if (!Physics::world_created) {
                Physics::initialize();
                Physics::world_created = true;
            }
            
            Component comp_new;
            RigidBody rb;
            
            
            b2BodyDef body_def;
            body_def.type = b2_kinematicBody;
            body_def.position = b2Vec2(pos.x, pos.y);
            
            comp_new.type = component_type;
            
            b2Body* body = Physics::__World->CreateBody(&body_def);
            
            // now do stuff with the fixture
            create_fixture(body, actor_new, rb);
            
            rb.body = body;
            
            //push rigidbody onto lua stack, then retrieve a reference
            luabridge::push(__State, rb);
            luabridge::LuaRef ref = luabridge::LuaRef::fromStack(__State, -1);
            comp_new.ref = std::make_shared<luabridge::LuaRef>(ref);
            
            
            actor_new->components.insert(std::make_pair(key, comp_new));
            actor_new->component_type_to_key[component_type].push_back(key);
            
        }
        //if there's no component of the type yet, load one
        else if (!component_type_exists(component_type)) {
            Component comp_new;
            comp_new.load_ref(component_type, __State);
            components[component_type] = comp_new;
        }

        //create a new component instance that will be owned by an actor
        Component actor_owned_component;
        create_child(key, actor_owned_component, components[component_type]);

        actor_new->attach_component(actor_owned_component, key);
        
        if (component_type == "Transform") {
            Component& comp_to_override = actor_new->components[key];
            comp_to_override.inject_variable("x", pos.x);
            comp_to_override.inject_variable("y", pos.y);
        }
        else if (component_type == "SpriteRenderer") {
            Component& comp_to_override = actor_new->components[key];
            std::string tile_name = actor_new->name + std::to_string(gid);
            comp_to_override.inject_variable("image", tile_name);
            int zero = 0;
            int one = 1;
            comp_to_override.inject_variable("pivot_x", zero);
            comp_to_override.inject_variable("pivot_y", one);
        }
    }
}

bool ComponentDB::component_type_exists(std::string& type) {
    return (components.find(type) != components.end());
}

void ComponentDB::establish_inheritance(luabridge::LuaRef& instance, luabridge::LuaRef& parent) {
    //create metatable
    luabridge::LuaRef metatable = luabridge::newTable(__State);
    metatable["__index"] = parent;

    instance.push(__State);
    metatable.push(__State);
    lua_setmetatable(__State, -2);
    lua_pop(__State, 1);
}

void ComponentDB::Log(const std::string &message) {
    std::cout << message << std::endl;
}

void ComponentDB::LogError(const std::string &error_message) {
    std::cerr << error_message << std::endl;
}

int ComponentDB::GetFrame() {
    return Helper::GetFrameNumber();
}

void ComponentDB::Quit() {
    std::exit(0);
}

void ComponentDB::Sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void ComponentDB::OpenURL(std::string url) {
    std::string command;
#ifdef _Win32
    command = "start ";
#elif defined(__APPLE__)
    command = "open ";
#else
    command = "xdg-open ";
#endif
    const std::string to_call = command + url;
    std::system(to_call.c_str());
}
