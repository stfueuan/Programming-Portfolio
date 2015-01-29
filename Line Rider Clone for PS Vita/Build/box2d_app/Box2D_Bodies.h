#ifndef BODIES_H
#define BODIES_H

#include <Box2D\Box2D.h>
#include <math.h>

b2Body* DynamicBody(b2Vec2 initial_position, float dynamic_body_half_width, float dynamic_body_half_height, float dynamic_density, float dynamic_friction, float dynamic_restitution, b2World* world);
b2Body* StaticBody(b2Vec2 initial_position, float static_half_width, float static_half_height, b2World* world);
b2Body* StaticBody_sensorless(b2Vec2 initial_position, float static_half_width, float static_half_height, b2World* world);
b2Body* CircleBody(b2Vec2 initial_position, float radius, float dynamic_density, float dynamic_friction, float dynamic_restitution, b2World* world);
b2Body* EdgeShape(b2Vec2 v1, b2Vec2 v2, b2World* world);
b2MouseJoint* MouseJoint(b2Vec2 target, b2Body* ground, b2Body* body, b2World* world);
b2RevoluteJoint* RevoluteJoint(b2Body* bodyA, b2Body* bodyB, b2Vec2 Local_anchor_a, b2Vec2 Local_anchor_b, float reference_angle, b2World* world);

#endif // BODIES_H