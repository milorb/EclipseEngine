#include "Physics.hpp"
#include "RigidBody.hpp"

b2BodyDef RigidBody::initialize(const rapidjson::Value& rb) {
    if (!rb.IsNull()) {

        if (rb.HasMember("x")) {
            x = rb["x"].GetFloat();
        }
        if (rb.HasMember("y")) {
            y = rb["y"].GetFloat();
        }
        if (rb.HasMember("width")) {
            width = rb["width"].GetFloat();
        }
        if (rb.HasMember("height")) {
            height = rb["height"].GetFloat();
        }
        if (rb.HasMember("radius")) {
            radius = rb["radius"].GetFloat();
        }
        if (rb.HasMember("friction")) {
            friction = rb["friction"].GetFloat();
        }
        if (rb.HasMember("bounciness")) {
            bounciness = rb["bounciness"].GetFloat();
        }
        if (rb.HasMember("density")) {
            density = rb["density"].GetFloat();
        }
        if (rb.HasMember("rotation")) {
            rotation = rb["rotation"].GetFloat();
        }
        if (rb.HasMember("gravity_scale")) {
            gravity_scale = rb["gravity_scale"].GetFloat();
        }
        if (rb.HasMember("angular_friction")) {
            angular_friction = rb["angular_friction"].GetFloat();
        }
        if (rb.HasMember("precise")) {
            precise = rb["precise"].GetBool();
        }
        if (rb.HasMember("body_type")) {
            body_type = rb["body_type"].GetString()[0];
        }
        if (rb.HasMember("has_collider")) {
            has_collider = rb["has_collider"].GetBool();
        }
        if (rb.HasMember("collider_type")) {
            collider_type = rb["collider_type"].GetString();
        }
        if (rb.HasMember("has_trigger")) {
            has_trigger = rb["has_trigger"].GetBool();
        }
        if (rb.HasMember("trigger_type")) {
            trigger_type = rb["trigger_type"].GetString();
        }
        if (rb.HasMember("trigger_width")) {
            trigger_width = rb["trigger_width"].GetFloat();
        }
        if (rb.HasMember("trigger_height")) {
            trigger_height = rb["trigger_height"].GetFloat();
        }
        if (rb.HasMember("trigger_radius")) {
            trigger_radius = rb["trigger_radius"].GetFloat();
        }
    }
    
    b2BodyDef body_def;
    
    switch (body_type) {
        case 's': 
            body_def.type = b2_staticBody;
            break;
        case 'd': body_def.type = b2_dynamicBody;
            break;
        case 'k': body_def.type = b2_kinematicBody;
            break;
        default:
            std::cout << "body_type not recognized";
            std::exit(0);
    }
    
    body_def.position.x = x;
    body_def.position.y = y;
    
    body_def.bullet = precise;
    
    body_def.gravityScale = gravity_scale;
    body_def.angularDamping = angular_friction;
    body_def.angle = rotation * (b2_pi / 180.0f); //convert the degrees to radians
    
    return body_def;
}

void RigidBody::AddForce(b2Vec2 force) {
    body->ApplyForceToCenter(force, true);
}


b2Vec2 RigidBody::GetPosition() {
    if (!body) {
        return b2Vec2(x, y);
    }
    return body->GetPosition();
}
void RigidBody::SetPosition(b2Vec2 new_pos) {
    if (!body) {
        x = new_pos.x;
        y = new_pos.y;
    }
    else {
        body->SetTransform(new_pos, body->GetAngle());
    }
}


b2Vec2 RigidBody::GetVelocity() {
    if (!body) {
        return b2Vec2(0, 0);
    }
    return body->GetLinearVelocity();
}
void RigidBody::SetVelocity(b2Vec2 vel) {
    if (!body) {
        return;
    }
    body->SetLinearVelocity(vel);
}


float RigidBody::GetAngularVelocity() {
    if (!body) {
        return 0.0f;
    }
    return body->GetAngularVelocity() * (180.0f / b2_pi);
}
void RigidBody::SetAngularVelocity(float clockwise_deg) {
    if (!body) {
        return;
    }
    float rads = clockwise_deg * (b2_pi / 180.0f);
    body->SetAngularVelocity(rads);
}


float RigidBody::GetGravityScale() {
    if (!body) {
        return gravity_scale;
    }
    return body->GetGravityScale();
}
void RigidBody::SetGravityScale(float grav) {
    if (!body) {
       gravity_scale = grav;
       return;
    }
    body->SetGravityScale(grav);
}


//DONT TOUCH
b2Vec2 RigidBody::GetUpDirection() {
    float cur_angle_rads = body->GetAngle();
    b2Vec2 up(glm::sin(cur_angle_rads), -glm::cos(cur_angle_rads));
    up.Normalize();
    return up;
}

void RigidBody::SetUpDirection(b2Vec2 dir) {
    dir.Normalize();
    float rads = glm::atan(dir.x, -dir.y);
    body->SetTransform(body->GetPosition(), rads);
}

b2Vec2 RigidBody::GetRightDirection() {
    float cur_angle_rads = body->GetAngle() + (b2_pi / 2.0f);
    b2Vec2 rt(glm::sin(cur_angle_rads), -glm::cos(cur_angle_rads));
    rt.Normalize();
    return rt;
}
void RigidBody::SetRightDirection(b2Vec2 dir) {
    dir.Normalize();
    float rads = glm::atan(dir.x, -dir.y) - (b2_pi / 2.0f);
    body->SetTransform(body->GetPosition(), rads);
}

float RigidBody::GetRotation() {
    if (!body) {
        return rotation;
    }
    return body->GetAngle() * (180.0f / b2_pi); //back to degrees
}

void RigidBody::SetRotation(float clockwise_deg) {
    float rads = clockwise_deg * (b2_pi / 180.0f);
    body->SetTransform(body->GetPosition(), rads);
}


void RigidBody::OnDestroy() {
    Physics::__World->DestroyBody(body);
}

void RigidBody::OnStart() {
    b2BodyDef def = initialize(rapidjson::Value());

    if (!Physics::world_created) {
        Physics::initialize();
        Physics::world_created = true;
    }

    body = Physics::__World->CreateBody(&def);

    ComponentDB::create_fixture(body, owner, *this);
}
