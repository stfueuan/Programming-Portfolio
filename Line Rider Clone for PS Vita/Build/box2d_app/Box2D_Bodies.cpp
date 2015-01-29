#include "Box2D_Bodies.h"

b2Body* DynamicBody(b2Vec2 initial_position, float dynamic_body_half_width, float dynamic_body_half_height, float dynamic_density, float dynamic_friction, float dynamic_restitution, b2World* world)
{
	b2BodyDef bodyDef;
	b2Body* projectile_body_;
	bodyDef.type = b2_dynamicBody; 
	bodyDef.position = initial_position;
	projectile_body_ = world->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(dynamic_body_half_width, dynamic_body_half_height); //set shape to be a square

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = dynamic_density;
	fixtureDef.friction = dynamic_friction;
	fixtureDef.restitution = dynamic_restitution;
	projectile_body_->CreateFixture(&fixtureDef);

	return projectile_body_;
}

b2Body* StaticBody(b2Vec2 initial_position, float static_half_width, float static_half_height, b2World* world)
{
	b2BodyDef ground_body_def;
	b2Body* ground_body;
	ground_body_def.type = b2_staticBody;
	ground_body_def.position = initial_position;
	ground_body = world->CreateBody(&ground_body_def);

	b2PolygonShape ground_shape;
	ground_shape.SetAsBox(static_half_width, static_half_height); //set shape to be a square

	ground_body->CreateFixture(&ground_shape, 0.0f);

	return ground_body;
}

b2Body* StaticBody_sensorless(b2Vec2 initial_position, float static_half_width, float static_half_height, b2World* world)
{
	b2BodyDef ground_body_def;
	b2Body* ground_body;

	ground_body_def.type = b2_staticBody;
	ground_body_def.position = initial_position;
	ground_body = world->CreateBody(&ground_body_def);

	b2PolygonShape ground_shape;
	ground_shape.SetAsBox(static_half_width, static_half_height); //set the shape to be a square

	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true; //lets all bodies pass through it as if it does not exist
	fixtureDef.shape = &ground_shape;
	ground_body->CreateFixture(&fixtureDef);

	return ground_body;
}

b2Body* CircleBody(b2Vec2 initial_position, float radius, float dynamic_density, float dynamic_friction, float dynamic_restitution, b2World* world)
{
	b2BodyDef cicle_body_def;
	b2Body* circle_body;
    cicle_body_def.type = b2_dynamicBody;
    cicle_body_def.position = initial_position;
    circle_body = world->CreateBody(&cicle_body_def);

    b2CircleShape circle;
    circle.m_radius = radius;

    b2FixtureDef FixtureDef;
    FixtureDef.density = dynamic_density;
    FixtureDef.friction = dynamic_friction;
    FixtureDef.restitution = dynamic_restitution;
    FixtureDef.shape = &circle; //set the shape to be a circle

    circle_body->CreateFixture(&FixtureDef);

	return circle_body;
}

b2Body* EdgeShape(b2Vec2 v1, b2Vec2 v2, b2World* world)
{
	b2BodyDef edge_shape;
	b2Body* line;
	line = world->CreateBody(&edge_shape);

	b2EdgeShape edge;
	edge.Set(v1, v2);

	b2FixtureDef FixtureDef;
	FixtureDef.shape = &edge; //set the shape to be an edge

	line->CreateFixture(&FixtureDef);

	return line;
}

b2MouseJoint* MouseJoint(b2Vec2 target, b2Body* ground, b2Body* body, b2World* world)
{
	b2MouseJointDef mouse_joint;
	mouse_joint.bodyA = ground; //needs a body to anchor itself to within the world, like a dog on a lead
	mouse_joint.bodyB = body; //the body you want to move
	mouse_joint.target.Set(target.x, target.y); //where the body is to move to
	mouse_joint.maxForce = 1000.0f; //force applied to body for movement, higher the force, faster it moves

	b2MouseJoint* mouse = (b2MouseJoint*)world->CreateJoint(&mouse_joint);

	return mouse;
}

b2RevoluteJoint* RevoluteJoint(b2Body* bodyA, b2Body* bodyB, b2Vec2 Local_anchor_a, b2Vec2 Local_anchor_b, float reference_angle, b2World* world)
{	
	b2RevoluteJointDef revoluteJointDef;
	revoluteJointDef.bodyA = bodyA; //try to always make the main object that other objects will be attached to
	revoluteJointDef.bodyB = bodyB; //object that is to be attached to something
	revoluteJointDef.collideConnected = false; //will it collide with what it is attached to?
	revoluteJointDef.localAnchorA.Set(Local_anchor_a.x, Local_anchor_a.y); //car body
	revoluteJointDef.localAnchorB.Set(Local_anchor_b.x, Local_anchor_b.y); //wheel
	revoluteJointDef.referenceAngle = reference_angle; //default 0, sets what angle the body starts with
	revoluteJointDef.Initialize(bodyA, bodyB, bodyB->GetWorldCenter()); //give the bodies an initial position
	revoluteJointDef.enableMotor = false; //does the revolute joint have a motor?
	
	//Could add angle limits here when needed but not now as this is needed as an axle
	revoluteJointDef.enableLimit = false; //no limit for rotation

	b2RevoluteJoint* revolute_joint = (b2RevoluteJoint*)world->CreateJoint(&revoluteJointDef);

	return revolute_joint;
}