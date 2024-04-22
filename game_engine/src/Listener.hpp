#ifndef Listener_hpp
#define Listener_hpp

#include "box2d.h"
#include "Actor.hpp"

struct Collision {
    Collision(Actor* a, b2Vec2 p, b2Vec2 rv, b2Vec2 n) : 
        other(a), 
        point(p), 
        relative_velocity(rv), 
        normal(n) {}

    Actor* other;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

class Listener : public b2ContactListener 
{
    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;
};

#endif