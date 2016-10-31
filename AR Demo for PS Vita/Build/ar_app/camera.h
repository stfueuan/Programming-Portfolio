#ifndef CAMERA_H
#define CAMERA_H

#include <system/platform.h>
#include <maths/vector2.h>
#include <maths/vector3.h>
#include <vector>
#include <maths/math_utils.h>
#include <maths/matrix44.h>
#include <system/vita/platform_vita.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>

namespace abfw
{
	class Platform;
}

class Camera
{
public:
	Camera(abfw::Platform& platform);
	~Camera();

	void Init();
	abfw::Matrix44 GetProjectionMatrix();
	abfw::Matrix44 GetViewMatrix();

private:
	abfw::Vector3 camera_eye_;
	abfw::Vector3 camera_lookat_;
	abfw::Vector3 camera_up_;
	float camera_fov_;
	float near_plane_;
	float far_plane_;
	abfw::Matrix44 projection_matrix;
	abfw::Matrix44 view_matrix;

	abfw::Platform* platform_;
};

#endif /*CAMERA_H*/