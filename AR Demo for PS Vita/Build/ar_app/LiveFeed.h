#ifndef LIVE_FEED_H
#define LIVE_FEED_H

#include <system/platform.h>
#include <graphics/sprite.h>
#include <graphics/vita/texture_vita.h>
#include <vector>

// Vita AR includes
#include <camera.h>
#include <gxm.h>
#include <motion.h>
#include <libdbg.h>
#include <libsmart.h>
#include <sony_sample_framework.h>
#include <sony_tracking.h>

class LiveFeed
{
public:
	struct MarkerTransforms
	{
		abfw::Matrix44 transform;
		int markerID;
	};

	LiveFeed(abfw::Platform& platform);
	~LiveFeed();

	void Init();
	std::vector<MarkerTransforms> Update();
	void BeginRender();
	void Reset();
	abfw::Sprite GetCameraSprite();
	abfw::Matrix44 GetOrthoMatrix();
	abfw::Matrix44 GetProjectionMatrix();
	abfw::Matrix44 GetViewMatrix();

private:
	abfw::Platform* platform_;
	abfw::TextureVita* camera_texture_;
	abfw::Sprite camera_feed_sprite_;
	abfw::Matrix44 orthographicProjectionMatrix;
	float yScreenScale;
	abfw::Matrix44 live_camera_perspective_matrix_;
	abfw::Matrix44 live_camera_view_matrix_;
};

#endif

