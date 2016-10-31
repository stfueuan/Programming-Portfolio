#include "gameObject.h"
#include <maths/quaternion.h>
#include <iostream>

GameObject::GameObject(abfw::Platform& platform)
{
	platform_ = &platform;
}

GameObject::~GameObject()
{
	delete platform_;
	platform_ = 0;

	//model_->Release();

	delete model_;
	model_ = 0;

	delete mesh_;
	mesh_ = 0;
}

void GameObject::Init(abfw::Model &model, const abfw::Vector3 initialPosition)
{
	//initialise gameobject instance with a given model and starting position
	model_ = &model;
	set_mesh(model_->mesh());
	initialPosition_ = initialPosition;
	Reset(initialPosition_);
}

void GameObject::Init(abfw::Mesh &mesh, const abfw::Vector3 initialPosition)
{
	//initialise gameobject instance with a given mesh and starting position
	mesh_ = &mesh;
	set_mesh(mesh_);
	initialPosition_ = initialPosition;
	Reset(initialPosition_);
}

void GameObject::Update(float frameTime)
{
	//If the object has a velocity, keep it moving
	Move(velocity * frameTime);

	//Make sure that the markers relative rotation is kept up to date
	RotateForward();
}

void GameObject::SetModelBoundingSphereRadius(float radius)
{
	//initialise sphere bounds for gameobject, passed in variable must be in meters
	abfw::Aabb aabb(abfw::Vector3(-1.0f, -1.0f, -1.0f), abfw::Vector3(1.0f, 1.0f, 1.0f));
	abfw::Sphere bounding_sphere(aabb);
	bounding_sphere.set_radius(radius);
	bounding_sphere.set_position(abfw::Vector3(0.0f, 0.0f, 0.0f));

	if (model_)
	{
		model_->mesh()->set_bounding_sphere(bounding_sphere);
	}
	else
	{
		mesh_->set_bounding_sphere(bounding_sphere);
	}
}

bool GameObject::AABBCollision(abfw::MeshInstance cube2)
{
	//Simple AABB collision detection
	//Checking collision with gameobject instance calling the function and the instance passed in
	//This check taken from here: http://www.miguelcasillas.com/?p=30 (accessed 27/01/16)	

	abfw::Matrix44 transformt, scale, rotX, rotZ;
	
	//Albeit not transferable, the only collision detection in the game is relative to the car - need to do the same transformations done at render time
	transformt.SetTranslation(local_transform.GetTranslation());
	scale_matrix.Scale(abfw::Vector3(0.01f, 0.01f, 0.01f));
	rotX.RotationX(FRAMEWORK_PI / 2.0f);
	rotZ.RotationZ(FRAMEWORK_PI);
	scale.Scale(abfw::Vector3(0.01f, 0.01f, 0.01f));

	transformt = scale * rotX * rotZ * transform();

	abfw::Aabb aabb1 = mesh()->aabb().Transform(transformt);
	abfw::Aabb aabb2 = cube2.mesh()->aabb().Transform(cube2.transform());
	
	bool collision = (aabb1.max_vtx().x > aabb2.min_vtx().x &&
					  aabb1.min_vtx().x < aabb2.max_vtx().x &&
					  aabb1.max_vtx().y > aabb2.min_vtx().y &&
					  aabb1.min_vtx().y < aabb2.max_vtx().y &&
					  aabb1.max_vtx().z > aabb2.min_vtx().z &&
					  aabb1.min_vtx().z < aabb2.max_vtx().z);

	return collision;
}

bool GameObject::SphereCollision(abfw::MeshInstance sphere2)
{
	//Simple bounding sphere collision detection
	float r1 = mesh()->bounding_sphere().radius();
	float r2 = sphere2.mesh()->bounding_sphere().radius();
	//Get the Euclidean distance between the two objects and check it against the radii
	float r = r1 + r2;
	r *= r;

	abfw::Vector3 dist = sphere2.transform().GetTranslation() - transform().GetTranslation();

	//Not square rooting saves performance
	float distance = dist.x * dist.x + dist.y * dist.y + dist.z * dist.z;

	if (distance <= r)
	{
		return true;
	}

	return false;
}

void GameObject::Move(const abfw::Vector3 movementVector)
{
	//Get the local y rotation in order to rotate the movement vector to move the car in the direction it is facing
	abfw::Vector3 tempVector;

	tempVector.x = local_transform.m[1][0];
	tempVector.y = local_transform.m[1][1];
	tempVector.z = local_transform.m[1][2];
	tempVector *= movementVector.y;

	//Now translate the car by x units in the direction it is facing
	local_transform.SetTranslation(local_transform.GetTranslation() + tempVector + abfw::Vector3(0.0f, 0.0f, movementVector.z));
}

void GameObject::Scale(const abfw::Vector3 scaleVector)
{
	abfw::Matrix44 temp;
	temp.Scale(scaleVector);
	local_transform = local_transform * temp;
}

void GameObject::RotateX(float x)
{
	abfw::Matrix44 temp;
	temp.RotationX(x);
	local_transform = local_transform * temp;
}

void GameObject::RotateY(float y)
{
	abfw::Matrix44 temp;
	temp.RotationY(y);
	local_transform = local_transform * temp;
}

void GameObject::RotateZ(float z)
{
	abfw::Matrix44 temp;
	temp.RotationZ(z);
	local_transform = local_transform * temp;
}

void GameObject::RotateLocalZ(float z)
{
	abfw::Matrix44 temp, temp2, temp3;
	temp.SetIdentity();
	temp.RotationZ(z);
	temp2 = local_transform;
	temp2.RotationX(0.0f);
	temp2.RotationY(0.0f);

	abfw::Quaternion currentQuat, newQuat, actualQuat;

	currentQuat.SetFromMatrix(temp2);
	newQuat.SetFromMatrix(temp);

	currentQuat.Lerp(currentQuat, newQuat, 1.0f);
	currentQuat.Normalise();
	temp3.SetIdentity();
	temp3.Rotation(currentQuat);

	abfw::Vector3 translation = local_transform.GetTranslation();

	local_transform = local_transform * temp3;

	local_transform.SetTranslation(translation);
}

void GameObject::UpdateTransform()
{
	/*abfw::Matrix44 temp;
	temp = scale_matrix * rotation_matrix * translation_matrix;
	set_transform(temp);*/
}

void GameObject::SetVelocity(const abfw::Vector3 velocityVector)
{
	velocity = velocityVector;
}

void GameObject::SetZVelocity(float z)
{
	//Typically used to add or remove gravity
	velocity.z = z;
}

void GameObject::SetTransform(std::vector<LiveFeed::MarkerTransforms> marker_transforms)
{
	//This function originally detected the closest marker and set the car transform to it
	//Now just looks for marker 0, otherwise whatever marker is availble
	set_transform(local_transform * marker_transforms[0].transform);
}

void GameObject::Accelerate(float frameTime)
{
	const abfw::Vector3 changeInVelocity = abfw::Vector3(0.0f, 0.05f, 0.0f);
	abfw::Vector3 currentVelocity = velocity;

	//Make sure acceleration is appropriately incremental
	currentVelocity += changeInVelocity * frameTime;

	//No speeding!
	if (currentVelocity.y > 0.25f)
	{
		currentVelocity.y = 0.25f;
	}

	SetVelocity(currentVelocity);

	//If we're accelerating, no need to reverse
	reverseTimer = 0.0f;
}

void GameObject::Decelerate(float frameTime, bool reverse)
{
	const abfw::Vector3 changeInVelocity = abfw::Vector3(0.0f, 0.07f, 0.0f);
	abfw::Vector3 currentVelocity = velocity;

	currentVelocity -= changeInVelocity * frameTime;

	if (currentVelocity.y < 0.0f)
	{
		//Act like a Need for Speed game and use the same button for brake and reverse but with a timer after the car has stopped accelerating
		if (reverse)
			reverseTimer += frameTime * 10.0f;

		if (reverseTimer < 1.0f)
			currentVelocity.y = 0.0f;

		//No speeding in reverse!
		if (currentVelocity.y < -0.15f)
		{
			currentVelocity.y = -0.15f;
		}
	}

	if (!reverse)
	{
		reverseTimer = 0.0f;
	}

	SetVelocity(currentVelocity);
}

abfw::Vector3 GameObject::GetVelocity()
{
	return velocity;
}

void GameObject::Reset(abfw::Vector3 initialPosition)
{
	//Be sure to return the car to its original starting position, scale, rotation etc.
	translation_matrix.SetIdentity();
	translation_matrix.SetTranslation(initialPosition);
	set_transform(translation_matrix);

	velocity = abfw::Vector3(0.0f, 0.0f, 0.0f);
	translation_matrix.SetIdentity();
	scale_matrix.SetIdentity();
	rotation_matrix.SetIdentity();

	temp_prev_matrix.SetIdentity();
	local_transform.SetIdentity();
	
	SetResetBool(true);
}

float GameObject::GetVelocityPercentage()
{
	//The maximum velocity of the car is 0.25f ms-1, take the current velocity and divide by the max to get the percentage - used for the speedometer UI
	return velocity.y / 0.25f;
}

void GameObject::RotateForward()
{
	//In order to calculate the relative rotation of each marker, a vector, in this case the forward vector, is rotated based on the markers rotation
	abfw::Matrix44 temp, temp2;
	abfw::Quaternion quat;
	temp.SetIdentity();
	temp2.SetIdentity();

	forward = abfw::Vector3(0.0f, 1.0f, 0.0f);

	temp2.SetTranslation(forward);
	
	quat.SetFromMatrix(transform());
	temp.Rotation(quat);
	temp.SetTranslation(forward);

	temp2 = temp2 * temp;

	//After rotating the vector, normalise and send it out
	forward = temp2.GetTranslation();
	forward.Normalise();
}

float GameObject::GetMajorRotation()
{
	//An extension of the RotateForward function
	//Since the track 'snaps' into a locked grid, the track pieces must only rotate by 90 degree intervals
	//By taking the arctan of the forward vector in the x and y axis, we get an angle between 0 and 2PI
	//After a quick conversion into Euler angles for readability, determine which multiple of 90 degrees is currently relevant 
	float rot = std::atan2(forward.y, forward.x);
	float tempA = 2.0f * rot * FRAMEWORK_RAD_TO_DEG;

	if (tempA >= 0.0f && tempA < 90.0f)
	{
		rot = 0.0f;
	}
	else if (tempA >= 90.0f && tempA < 180.0f)
	{
		rot = FRAMEWORK_PI / 2.0f;
	}
	else if (tempA >= 180.0f && tempA < 270.0f)
	{
		rot = FRAMEWORK_PI;
	}
	else
	{
		rot = 3.0f * FRAMEWORK_PI / 2.0f;
	}

	return rot;
}