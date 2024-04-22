#include "Listener.hpp"

void Listener::BeginContact(b2Contact* contact) {
    b2Fixture* fx_A = contact->GetFixtureA();
    b2Fixture* fx_B = contact->GetFixtureB();

    Actor* A = reinterpret_cast<Actor*>(fx_A->GetUserData().pointer);
    Actor* B = reinterpret_cast<Actor*>(fx_B->GetUserData().pointer);

    b2Vec2 vel_rel = fx_A->GetBody()->GetLinearVelocity() -
                          fx_B->GetBody()->GetLinearVelocity();

    /*b2Vec2 vel_B_minus_A = fx_B->GetBody()->GetLinearVelocity() -
                          fx_A->GetBody()->GetLinearVelocity();*/

    if (fx_A->GetFilterData().categoryBits == 
        fx_B->GetFilterData().categoryBits) {
        //is collider
        if (fx_A->GetFilterData().categoryBits == 0x0002) {

            b2WorldManifold manifold;
            contact->GetWorldManifold(&manifold);

            Collision collision_A(B,
                manifold.points[0],
                vel_rel,
                manifold.normal);

            Collision collision_B(A,
                manifold.points[0],
                vel_rel,
                manifold.normal);

            A->EnterCollision(collision_A);
            B->EnterCollision(collision_B);
        }

        //is trigger
        if (fx_A->GetFilterData().categoryBits == 0x0070) {
            
            b2Vec2 sentinel(-999.0f, -999.0f);

            Collision collision_A(B,
                sentinel,
                vel_rel,
                sentinel);

            Collision collision_B(A,
                sentinel,
                vel_rel,
                sentinel);

            A->EnterTrigger(collision_A);
            B->EnterTrigger(collision_B);
        } 
    }
}

void Listener::EndContact(b2Contact* contact) {
    b2Fixture* fx_A = contact->GetFixtureA();
    b2Fixture* fx_B = contact->GetFixtureB();

    Actor* A = reinterpret_cast<Actor*>(fx_A->GetUserData().pointer);
    Actor* B = reinterpret_cast<Actor*>(fx_B->GetUserData().pointer);

    b2Vec2 vel_rel = fx_A->GetBody()->GetLinearVelocity() -
                          fx_B->GetBody()->GetLinearVelocity();

    /*b2Vec2 vel_B_minus_A = fx_B->GetBody()->GetLinearVelocity() - 
                          fx_A->GetBody()->GetLinearVelocity();*/

    b2Vec2 sentinel(-999.0f, -999.0f);
    Collision collision_A(B,
        sentinel,
        vel_rel,
        sentinel);

    Collision collision_B(A,
        sentinel,
        vel_rel,
        sentinel);

    if (fx_A->GetFilterData().categoryBits ==
        fx_B->GetFilterData().categoryBits) {
        //is collider
        if (fx_A->GetFilterData().categoryBits == 0x0002) {
            A->ExitCollision(collision_A);
            B->ExitCollision(collision_B);
        }
        //is trigger
        if (fx_A->GetFilterData().categoryBits == 0x0070) {
            A->ExitTrigger(collision_A);
            B->ExitTrigger(collision_B);
        }
    }
}