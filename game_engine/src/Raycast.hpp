#ifndef Raycast_hpp
#define Raycast_hpp

#include <vector>

#include "box2d.h"

#include "Actor.hpp"

struct RayData {
    b2Fixture* fix;
    b2Vec2 point;
    b2Vec2 normal;
    float frac;

    bool frac_sort(const RayData& other) {
        return frac < other.frac;
    }
};

class NearestRaycast : public b2RayCastCallback {
public:
	float ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float fraction) override;

    b2Fixture* fix;
    b2Vec2 point;
    b2Vec2 normal;
    float fraction;
};

class AllRaycast : public b2RayCastCallback {
public:
    float ReportFixture(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction) override;

    std::vector<RayData> hit_data;
};

#endif
