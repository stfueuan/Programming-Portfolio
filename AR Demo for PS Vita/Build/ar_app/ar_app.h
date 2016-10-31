#ifndef _RENDER_TARGET_APP_H
#define _RENDER_TARGET_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector3.h>
#include <maths/quaternion.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <graphics/vita/texture_vita.h>
#include <assets/obj_loader.h>
#include <assets/png_loader.h>

// Custom includes
#include "gameObject.h"
#include "controller_input.h"
#include "camera.h"
#include "LiveFeed.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace abfw
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class TextureVita;
	class OBJMesh;
	class OBJLoader;
	class SonyControllerInputManager;
}

class ARApp : public abfw::Application
{
public:
	ARApp(abfw::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void RenderCameraFeed(struct AppData* dat);
	void Render3DScene();
	void RenderOverlay();
	abfw::Mesh* CreatePlaneMesh(abfw::TextureVita*, abfw::Vector3 scale, int planeType = 1);
	abfw::Mesh* CreateCubeMesh(float size); //meters
	void BuildTrack(GameObject** trackPieces);
	void PlaceTrackPieces(GameObject* centrePiece, GameObject* trackPiece, int index);
	void ResetTimer();
	abfw::TextureVita* LoadTexture(abfw::TextureVita* outTexture, const char* fileName);
	
	abfw::SonyControllerInputManager* input_manager_;

	abfw::SpriteRenderer* sprite_renderer_;
	abfw::Font* font_;

	float fps_;

	class abfw::Renderer3D* renderer_3d_;

	abfw::Model* car;
	GameObject car_gameObject;
	
	abfw::Mesh* car_shadow_mesh;
	abfw::TextureVita* shadow_texture;
	abfw::TextureVita* road_texture;
	GameObject car_shadow_gameObject;

	abfw::Mesh* track_mesh;
	GameObject* track_gameObject[6];

	ControllerInput controller_input;

	bool render[6];
	bool* reconfigureLiveFeed;

	LiveFeed liveCameraFeed;
	std::vector<LiveFeed::MarkerTransforms> marker_transforms;
	std::vector<bool> collision_bool;

	struct Track
	{
		int markerID = 0;
		int x = 0;
		int y = 0;

		Track(int initMarkerID, int initX, int initY)
		{
			markerID = initMarkerID;
			x = initX;
			y = initY;
		}
	};

	std::vector<int> notTrack;
	std::vector<Track> track;

	abfw::Sprite* speedoNotches;
	abfw::Sprite* speedoNumbers;
	abfw::Sprite* speedoNeedle;
	abfw::PNGLoader png_loader;
	abfw::TextureVita* speedoSprite;
	abfw::TextureVita* speedoNeedleSprite;

	abfw::Mesh* raceBlock;
	GameObject raceGameObject;

	struct Times
	{
		float raceMilliseconds = 0.0f;
		int raceSeconds = 0;
		int raceMinutes = 0;
		float raceTimer = 0.0f;
	};

	std::vector<Times> bestTimes;
	Times currentTime;
};




#endif // _RENDER_TARGET_APP_H