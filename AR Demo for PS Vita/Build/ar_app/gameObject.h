#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <system/platform.h>
#include <graphics/mesh_instance.h>
#include <graphics\mesh.h>
#include <graphics\model.h>
#include <maths\math_utils.h>

#include "gameObject.h"
#include "LiveFeed.h"

namespace abfw
{
	class Platform;
}

class GameObject : public abfw::MeshInstance
{
public:
	GameObject(abfw::Platform& platform);
	~GameObject();

	void Init(abfw::Model &model, const abfw::Vector3 intitialPosition);
	void Init(abfw::Mesh &mesh, const abfw::Vector3 intitialPosition);
	void SetModelBoundingSphereRadius(float radius);
	bool SphereCollision(abfw::MeshInstance sphere2);
	bool AABBCollision(abfw::MeshInstance cube2);
	void Move(const abfw::Vector3 movementVector);
	void Scale(const abfw::Vector3 scaleVector);
	void RotateX(float x);
	void RotateY(float y);
	void RotateZ(float z);
	void RotateLocalZ(float z);
	void UpdateTransform();
	void SetVelocity(const abfw::Vector3 velocityVector); //remember that velocity is a speed in a given direction
	void SetZVelocity(float z);
	void Update(float frameTime);
	void SetTransform(std::vector<LiveFeed::MarkerTransforms> marker_transforms);
	void Accelerate(float frameTime);
	void Decelerate(float frameTime, bool reverse = false);
	abfw::Vector3 GetVelocity();
	void Reset(abfw::Vector3 initialPosition);
	void RotateForward();
	float GetVelocityPercentage(); //Return a value between 0 and 1
	float GetMajorRotation();

	abfw::Vector3 GetForwardVector()
	{
		return forward;
	}

	abfw::Vector3 GetInitialPos()
	{
		return initialPosition_;
	}

	bool GetResetBool()
	{
		return reset;
	}

	void SetResetBool(bool reset_)
	{
		reset = reset_;
	}

private:
	abfw::Model* model_;
	abfw::Mesh* mesh_;
	abfw::Matrix44 translation_matrix;
	abfw::Matrix44 scale_matrix;
	abfw::Matrix44 rotation_matrix;
	abfw::Matrix44 temp_prev_matrix;
	abfw::Vector3 initialPosition_;
	abfw::Matrix44 local_transform;

	abfw::Platform* platform_;

	abfw::Vector3 velocity;
	int prevMarker = 0;
	float reverseTimer = 0.0f;
	bool reset = false;

	abfw::Vector3 forward = abfw::Vector3(0.0f, 1.0f, 0.0f);

};

#endif /*GAMEOBJECT_H*/