#include "Physics.hpp"
#include "Raycast.hpp"

float NearestRaycast::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
	const b2Vec2& normal, float fraction) {
	fix = fixture;
	this->point = point;
	this->normal = normal;
	this->fraction = fraction;

	return fraction;
}

float AllRaycast::ReportFixture(b2Fixture* fixture, const b2Vec2& point,
	const b2Vec2& normal, float fraction) {

	RayData rd;
	rd.fix = fixture;
	rd.point = point;
	rd.normal = normal;
	rd.frac = fraction;

	hit_data.emplace_back(rd);

	return 1;
}
