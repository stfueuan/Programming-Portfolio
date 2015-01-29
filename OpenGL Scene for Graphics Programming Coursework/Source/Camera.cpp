#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::init()
{
	Yaw = 0;
	Pitch = 0;
	Roll = 0;

	position.x = 0;
	position.y = 0;
	position.z = 6;
}

void Camera::CameraUpdate()
{
	float cosR, cosP, cosY; //temp values for sin/cos from 
	float sinR, sinP, sinY;

	// Roll, Pitch and Yaw are variables stored by the camera
	// handle rotation
	// Only Want to calculate these values once. 
	// For speeds sake. 
	cosY = cosf(Yaw*3.1415/180);
	cosP = cosf(Pitch*3.1415/180);
	cosR = cosf(Roll*3.1415/180);
	sinY = sinf(Yaw*3.1415/180);
	sinP = sinf(Pitch*3.1415/180);
	sinR = sinf(Roll*3.1415/180);

	// This using the parametric equation of a sphere
	// Calculate the three vectors to put into glu Lookat
	// Look direction, position and the up vector 
	// This function could also calculate the right vector
	forward.x = sinY * cosP;
	forward.y = sinP;
	forward.z = cosP * -cosY;

	// Look At Point
	// To calculate add Forward Vector to Camera position.
	// Up Vector
	up.x = -cosY * sinR - sinY * sinP * cosR;
	up.y = cosP * cosR;
	up.z = -sinY * sinR - sinP * cosR * -cosY;

	// Side Vector (right)
	//this is a cross product between the forward and up vector. 
	//If you don’t need to calculate this, don’t do it. 
	right.x = (up.y*forward.z) - (up.z*forward.y);
	right.y = (up.x*forward.z) - (up.z*forward.x);
	right.z = (up.x*forward.y) - (up.y*forward.x);
}

float Camera::GetPositionX()
{
	return position.x;
}

float Camera::GetPositionY()
{
	return position.y;
}

float Camera::GetPositionZ()
{
	return position.z;
}

void Camera::AddYaw()
{
	Yaw += 2;
}

void Camera::ReduceYaw()
{
	Yaw -= 2;
}

void Camera::AddPitch()
{
	Pitch += 2;
}

void Camera::ReducePitch()
{
	Pitch -= 2;
}

void Camera::AddRoll()
{
	Roll++;
}

void Camera::ReduceRoll()
{
	Roll--;
}

float Camera::GetRightX()
{
	return right.x;
}

float Camera::GetRightY()
{
	return right.y;
}

float Camera::GetRightZ()
{
	return right.z;
}

