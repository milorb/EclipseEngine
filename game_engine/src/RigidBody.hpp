#ifndef RigidBody_hpp
#define RigidBody_hpp

#include <stdio.h>
#include <memory>
#include <iostream>

#include "rapidjson/document.h"
#include "Actor.hpp"
#include "glm/glm.hpp"
#include "box2d.h"

class Physics;

class RigidBody {
public:
    
    b2BodyDef initialize(const rapidjson::Value& rb);
    
    void AddForce(b2Vec2 force);
    
    b2Vec2 GetPosition();
    void SetPosition(b2Vec2 new_pos);
    
    b2Vec2 GetVelocity();
    void SetVelocity(b2Vec2 vel);
    
    float GetAngularVelocity();
    void SetAngularVelocity(float clockwise_deg);
    
    float GetGravityScale();
    void SetGravityScale(float grav);
    
    b2Vec2 GetUpDirection();
    void SetUpDirection(b2Vec2 dir);
    
    b2Vec2 GetRightDirection();
    void SetRightDirection(b2Vec2 dir);
    
    float GetRotation();
    void SetRotation(float clockwise_deg);

    void OnStart();
    void OnDestroy();

    std::shared_ptr<Actor> owner;

    b2Body* body;

    bool enabled = true;
    
    bool has_trigger = true;
    bool has_collider = true;

    std::string key = "";
    
    std::string collider_type = "box";
    std::string trigger_type = "box";

    float x = 0.0f;
    float y = 0.0f;
    
    float density = 1.0f;

    float width = 1.0f;
    float trigger_width = 1.0f;

    float height = 1.0f;
    float trigger_height = 1.0f;

    float radius = 0.5f;
    float trigger_radius = 0.5f;

    float friction = 0.3f;
    float bounciness = 0.3f;

    float rotation = 0.0f;
    float gravity_scale = 1.0f;
    float angular_friction = 0.3f;
    

    bool precise = true;
    char body_type = 'd'; //(d)ynamic //(s)tatic //(k)inematic
};

#endif /* RigidBody_hpp */
