#include "RigidBody.hpp"
#include "Physics.hpp"

void Physics::init() {
    
    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor
                <void(*) (float, float)>()
        .addProperty("x",
                &b2Vec2::x)
        .addProperty("y", 
                &b2Vec2::y)
        .addFunction("Normalize", 
                &b2Vec2::Normalize)
        .addFunction("Length",
                &b2Vec2::Length)
        .addFunction("__add", 
                &b2Vec2::operator_add)
        .addFunction("__sub", 
                &b2Vec2::operator_sub)
        .addFunction("__mul", 
                &b2Vec2::operator_mul)
        .addStaticFunction("Distance", 
                &b2Distance)
        .addStaticFunction("Dot", 
                static_cast<float (*)
                           (const b2Vec2&,
                            const b2Vec2&)>
                           (&b2Dot))
        .endClass();
    
    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginClass<RigidBody>("RigidBody")
        .addFunction("AddForce",
                &RigidBody::AddForce)
        .addFunction("GetPosition",
                &RigidBody::GetPosition)
        .addFunction("SetPosition",
                &RigidBody::SetPosition)
        .addFunction("GetVelocity", 
                &RigidBody::GetVelocity)
        .addFunction("SetVelocity", 
                &RigidBody::SetVelocity)
        .addFunction("GetAngularVelocity",
                &RigidBody::GetAngularVelocity)
        .addFunction("SetAngularVelocity",
                &RigidBody::SetAngularVelocity)
        .addFunction("GetGravityScale",
                &RigidBody::GetGravityScale)
        .addFunction("SetGravityScale",
                &RigidBody::SetGravityScale)
        .addFunction("GetUpDirection",
                &RigidBody::GetUpDirection)
        .addFunction("SetUpDirection",
                &RigidBody::SetUpDirection)
        .addFunction("GetRightDirection",
                &RigidBody::GetRightDirection)
        .addFunction("SetRightDirection",
                &RigidBody::SetRightDirection)
        .addFunction("GetRotation",
                &RigidBody::GetRotation)
        .addFunction("SetRotation", 
                &RigidBody::SetRotation)
        .addFunction("OnDestroy",
                &RigidBody::OnDestroy)
        .addFunction("OnStart",
                &RigidBody::OnStart)

        .addProperty("x", 
          &RigidBody::x)
        .addProperty("y", 
          &RigidBody::y)
        .addProperty("density", 
          &RigidBody::density)
        .addProperty("width", 
          &RigidBody::width)
        .addProperty("trigger_width", 
          &RigidBody::trigger_width)
        .addProperty("height", 
          &RigidBody::height)
        .addProperty("trigger_height", 
          &RigidBody::trigger_height)
        .addProperty("radius", 
          &RigidBody::radius)
        .addProperty("trigger_radius", 
          &RigidBody::trigger_radius)
        .addProperty("friction", 
          &RigidBody::friction)
        .addProperty("bounciness", 
          &RigidBody::bounciness)
        .addProperty("rotation", 
          &RigidBody::rotation)  
        .addProperty("gravity_scale", 
          &RigidBody::gravity_scale) 
        .addProperty("angular_friction", 
          &RigidBody::angular_friction)  
        .addProperty("precise", 
          &RigidBody::precise) 
        .addProperty("body_type", 
          &RigidBody::body_type)
        .addProperty("key",
            &RigidBody::key)
        .addProperty("enabled",
            &RigidBody::enabled)
        .endClass();

    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginClass<Collision>("collision")
        .addProperty("other", 
                &Collision::other)
        .addProperty("point", 
                &Collision::point)
        .addProperty("relative_velocity", 
                &Collision::relative_velocity)
        .addProperty("normal", 
                &Collision::normal)
        .endClass();

    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginClass<HitResult>("HitResult")
        .addProperty("actor", 
                &HitResult::actor)
        .addProperty("point",
                &HitResult::point)
        .addProperty("normal", 
                &HitResult::normal)
        .addProperty("is_trigger", 
                &HitResult::is_trigger)
        .endClass();

    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginNamespace("Physics")
        .addFunction("Raycast", &Physics::DoRaycast)
        .addFunction("RaycastAll", &Physics::DoRaycastAll)
        .endNamespace();
}

void Physics::initialize() {
    b2Vec2 grav(0.0f, 9.8f);
    
    __World = std::make_shared<b2World>(grav);

    __Listener = std::make_shared<Listener>();

    __World->SetContactListener(&*__Listener);
}

void Physics::step() {
    if (world_created) {
        __World->Step(1.0f / 60.0f, 8, 3);
    }
}

luabridge::LuaRef Physics::DoRaycast(b2Vec2 pos, b2Vec2 dir, float dist) {

    NearestRaycast r;

    b2Vec2 dest = pos + (dist * dir);

    Physics::__World->RayCast(&r, pos, dest);

    HitResult hr;

    if (r.fix && r.fix->GetUserData().pointer != 0 && 
        r.fix->GetFilterData().categoryBits != 0x1000) {

        hr.actor = reinterpret_cast<Actor*>(r.fix->GetUserData().pointer);
        hr.point = r.point;
        hr.normal = r.normal;
        if (r.fix->GetFilterData().categoryBits == 0x0070) {
            hr.is_trigger = true;
        }
        else {
            hr.is_trigger = false;
        }

        luabridge::push(ComponentDB::__State, hr);
    }
    else {
        lua_pushnil(ComponentDB::__State);
    }

    return luabridge::LuaRef::fromStack(ComponentDB::__State);
}

luabridge::LuaRef Physics::DoRaycastAll(b2Vec2 pos, b2Vec2 dir, float dist) {

    AllRaycast r;

    b2Vec2 dest = pos + (dist * dir);

    Physics::__World->RayCast(&r, pos, dest);

    std::vector<RayData> sorted_data = r.hit_data; 

    std::sort(sorted_data.begin(), sorted_data.end(), [](RayData& a, RayData& b) {
        return a.frac_sort(b); });

  
    luabridge::LuaRef table = luabridge::newTable(ComponentDB::__State);
    int idx = 1;

    for (auto& res : sorted_data) {
        if (res.fix && res.fix->GetUserData().pointer != 0 && 
            res.fix->GetFilterData().categoryBits != 0x1000) {

            HitResult hr;
            hr.actor = reinterpret_cast<Actor*>(res.fix->GetUserData().pointer);
            hr.point = res.point;
            hr.normal = res.normal;
            if (res.fix->GetFilterData().categoryBits == 0x0070) {
                hr.is_trigger = true;
            }
            else {
                hr.is_trigger = false;
            }

            table[idx] = hr;
        }
        else {
            table[idx] = luabridge::newTable(ComponentDB::__State);
        }
        idx++;
    }
    return table;
}
