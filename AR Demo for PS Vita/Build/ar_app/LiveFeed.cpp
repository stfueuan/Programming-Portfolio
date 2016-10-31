#include "LiveFeed.h"


LiveFeed::LiveFeed(abfw::Platform& platform)
{
	platform_ = &platform; 

	camera_texture_ = new abfw::TextureVita();

	//Get scaling factor for camera feed
	float cameraAspectRatio = (float)CAMERA_WIDTH / (float)CAMERA_HEIGHT;
	float displayAspectRatio = (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT;
	float yScreenScale = displayAspectRatio / cameraAspectRatio;

	//Initialise orthographic projection
	/*
	Left edge = -1
	Right edge = 1
	Top edge = -1
	Bottom edge = 1
	Near distance = -1
	Far distance = 1
	*/

	orthographicProjectionMatrix.OrthographicFrustumVita(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	//Initialise the live cameras projection matrix
	live_camera_perspective_matrix_.PerspectiveFovVita(SCE_SMART_IMAGE_FOV, cameraAspectRatio, 0.01f, 10.0f);//2 meters from lens
	abfw::Matrix44 scale_matrix;
	scale_matrix.SetIdentity();
	scale_matrix.Scale(abfw::Vector3(1.0f, yScreenScale, 1.0f));
	live_camera_perspective_matrix_ = live_camera_perspective_matrix_ * scale_matrix;

	//Initialise the live cameras view matrix
	live_camera_view_matrix_.SetIdentity();

	//Initialise camera feed sprite
	camera_feed_sprite_.set_width(2.0f);
	camera_feed_sprite_.set_height(2.0f * yScreenScale);
	camera_feed_sprite_.set_position(abfw::Vector3(0.0f, 0.0f, 1.0f));
	camera_feed_sprite_.set_texture(camera_texture_);
}

LiveFeed::~LiveFeed()
{
	smartRelease();
	sampleRelease();

	delete camera_texture_;
	camera_texture_ = NULL;

	delete platform_;
	platform_ = NULL;
}

void LiveFeed::Init()
{
	// initialise sony framework
	sampleInitialize();
	smartInitialize();

	// reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);
}

std::vector<LiveFeed::MarkerTransforms> LiveFeed::Update()
{
	AppData* dat = sampleUpdateBegin();
	std::vector<MarkerTransforms> marker_tranforms;
	abfw::Matrix44 temp_transform;

	// use the tracking library to try and find markers
	smartUpdate(dat->currentImage);

	for (int marker_id = 0; marker_id < 6; marker_id++)
	{
		// check to see if a particular marker can be found
		if (sampleIsMarkerFound(marker_id))
		{
			// marker is being tracked, get it’s transform and push it into the vector
			sampleGetTransform(
				marker_id,
				&temp_transform);

			MarkerTransforms temp;
			temp.markerID = marker_id;
			temp.transform = temp_transform;
			marker_tranforms.push_back(temp);
		}
	}

	sampleUpdateEnd(dat);

	return marker_tranforms;
}

void LiveFeed::BeginRender()
{
	AppData* dat = sampleRenderBegin();

	// DRAW CAMERA FEED SPRITE HERE
	// check there is data present for the camera image before trying to draw it
	if (dat->currentImage)
	{
		camera_texture_->set_texture(dat->currentImage->tex_yuv);
		camera_feed_sprite_.set_texture(camera_texture_);
	}
	else
	{
		camera_feed_sprite_.set_texture(0);
	}
	
	sampleRenderEnd();
}

void LiveFeed::Reset()
{
	//Need to make sure that we are not reinitialising any smart pointers
	smartRelease();
	sampleRelease();

	// initialise sony framework
	sampleInitialize();
	smartInitialize();

	// reset marker tracking
	AppData* dat = sampleUpdateBegin();
	smartTrackingReset();
	sampleUpdateEnd(dat);
}

abfw::Sprite LiveFeed::GetCameraSprite()
{
	return camera_feed_sprite_;
}

abfw::Matrix44 LiveFeed::GetOrthoMatrix()
{
	return orthographicProjectionMatrix;
}

abfw::Matrix44 LiveFeed::GetProjectionMatrix()
{
	return live_camera_perspective_matrix_;
}

abfw::Matrix44 LiveFeed::GetViewMatrix()
{
	return live_camera_view_matrix_;
}