#ifndef Physics_hpp
#define Physics_hpp

#include <stdio.h>
#include <memory>
#include <string>

#include "lua.hpp"
#include "LuaBridge.h"

#include "box2d.h"
#include "ComponentDB.hpp"
#include "RigidBody.hpp"
#include "Listener.hpp"
#include "Raycast.hpp"


struct HitResult {
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger;
};


class Physics {
public:
    //always called
    static void init();
    //called in RigidBody if component exists
    static void initialize();

    static luabridge::LuaRef DoRaycast(b2Vec2 pos, b2Vec2 dir, float dist);

    static luabridge::LuaRef DoRaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);

    static void step();
    
    static inline bool world_created = false;
    static inline std::shared_ptr<b2World> __World;
    static inline std::shared_ptr<Listener> __Listener;
};

#endif /* Physics_hpp */
