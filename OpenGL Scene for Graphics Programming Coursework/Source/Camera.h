#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>

class Camera
{
public:
	Camera();
	~Camera();
	void init();
	void CameraUpdate();
	float GetPositionX(); //Get camera position
	float GetPositionY();
	float GetPositionZ();
	float GetRightX(); //Get right vector x
	float GetRightY(); 
	float GetRightZ();
	void AddYaw();
	void ReduceYaw();
	void AddPitch();
	void ReducePitch();
	void AddRoll();
	void ReduceRoll();

	struct Vector
	{
		float x;
		float y; 
		float z;
	};

	Vector up;
	Vector forward;
	Vector position;
	Vector right;

private:
	float Yaw, Pitch, Roll;
};

#endif