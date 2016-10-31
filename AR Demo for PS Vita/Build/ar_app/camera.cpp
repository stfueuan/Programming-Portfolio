#include "camera.h"

Camera::Camera(abfw::Platform& platform)
{
	platform_ = &platform;
}

Camera::~Camera()
{
	delete platform_;
	platform_ = 0;
}

void Camera::Init()
{
	// initialise the camera settings
	camera_eye_ = abfw::Vector3(0.0f, 50.0f, 500.0f);
	camera_lookat_ = abfw::Vector3(0.0f, 0.0f, 0.0f);
	camera_up_ = abfw::Vector3(0.0f, 1.0f, 0.0f);
	camera_fov_ = abfw::DegToRad(45.0f);
	near_plane_ = 0.01f;
	far_plane_ = 1000.f;

	projection_matrix = platform_->PerspectiveProjectionFov(camera_fov_, (float)platform_->width() / (float)platform_->height(), near_plane_, far_plane_);
	view_matrix.LookAt(camera_eye_, camera_lookat_, camera_up_);
}

abfw::Matrix44 Camera::GetProjectionMatrix()
{
	return projection_matrix;
}

abfw::Matrix44 Camera::GetViewMatrix()
{
	return view_matrix;
}