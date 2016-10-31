#include "ar_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/sony_controller_input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/render_target.h>
#include <graphics/vita/texture_vita.h>
#include <system/vita/file_vita.h>

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>

void *graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid);
void graphicsFree(SceUID uid);

ARApp::ARApp(abfw::Platform& platform) :
	Application(platform),
	input_manager_(NULL),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	shadow_texture(NULL),
	road_texture(NULL),
	liveCameraFeed(platform),
	controller_input(platform, car_gameObject, liveCameraFeed),
	car_gameObject(platform),
	car_shadow_gameObject(platform),
	speedoNotches(NULL),
	speedoNumbers(NULL),
	speedoNeedle(NULL),
	raceBlock(NULL),
	raceGameObject(platform),
	speedoNeedleSprite(NULL),
	speedoSprite(NULL)
{
}

void ARApp::Init()
{
	liveCameraFeed.Init();
	input_manager_ = platform_.CreateSonyControllerInputManager();
	sprite_renderer_ = platform_.CreateSpriteRenderer();
	renderer_3d_ = platform_.CreateRenderer3D();

	InitFont();

	//Get a marker mesh for rendering on top of the marker
	//Road texture accessed 20/04/16
	//http://texturelib.com/Textures/road/road/road_road_0021_01_preview.jpg
	road_texture = LoadTexture(road_texture, "road_road_0021_01_preview.png");
	for (int i = 0; i < 6; i++)
	{
		//being inefficient here because I want 4 corners
		if (i == 0 || i == 2 || i == 3 || i == 5)
			track_mesh = CreatePlaneMesh(road_texture, abfw::Vector3(3.0f, 3.0f, 1.0f), 2);
		else
			track_mesh = CreatePlaneMesh(road_texture, abfw::Vector3(3.0f, 3.0f, 1.0f));
		track_gameObject[i] = new GameObject(platform_);
		track_gameObject[i]->Init(*track_mesh, abfw::Vector3(0.0f, 0.0f, 0.0f));
	}

	//initialise model
	abfw::OBJLoader obj_loader;
	car = new abfw::Model;
	//Car model accessed 22/04/16
	//http://tf3dm.com/3d-model/old-car-80440.html
	obj_loader.Load("car.obj", platform_, *car);
	car_gameObject.Init(*car, abfw::Vector3(0.0f, 0.0f, 0.0f)); 

	//Initialise car shadow
	shadow_texture = LoadTexture(shadow_texture, "shadow.png");
	car_shadow_mesh = CreatePlaneMesh(shadow_texture, abfw::Vector3(0.5f, 1.5f, 1.0f), 1);
	car_shadow_gameObject.Init(*car_shadow_mesh, abfw::Vector3(0.0f, 0.0f, 0.0f));

	controller_input.Init();

	for (int i = 0; i < 6; i++)
	{
		render[i] = false;
	}

	//UI Stuff
	//Speedo image references - all accessed 21/04/16 - first two ended up being combined to save memory
	//http://speedhut.s3.amazonaws.com/temp/GR4-SPEEDO-16/GR4-SPEEDO-16_CENTURY-000000.png 
	//http://speedhut.s3.amazonaws.com/temp/GR4-SPEEDO-03/GR4-SPEEDO-03_STANDARD-000000.png 
	//http://i.imgur.com/Ptkct.png 	
	speedoNotches = new abfw::Sprite();
	speedoNotches->set_height(200.0f);
	speedoNotches->set_width(200.0f);
	speedoNotches->set_position(abfw::Vector3(860.0f, 444.0f, -0.9f));
	speedoSprite = LoadTexture(speedoSprite, "speedometer.png");
	speedoNotches->set_texture(speedoSprite);

	speedoNeedle = new abfw::Sprite();
	speedoNeedle->set_height(200.0f);
	speedoNeedle->set_width(200.0f);
	speedoNeedle->set_position(abfw::Vector3(860.0f, 444.0f, -0.9f));
	speedoNeedleSprite = LoadTexture(speedoNeedleSprite, "Ptkct.png");
	speedoNeedle->set_texture(speedoNeedleSprite);
	
	raceBlock = CreateCubeMesh(0.02f);
	raceGameObject.Init(*raceBlock, abfw::Vector3(0.0f, 0.0f, 0.0f));

	//Initialise some best times for the scoreboard
	for (int i = 1; i < 6; i++)
	{
		Times temp;
		temp.raceMilliseconds = 0.0f;
		temp.raceSeconds = 0;
		temp.raceMinutes = i * 2;
		temp.raceTimer = i * 120; //turn into seconds and multiply by 2

		bestTimes.push_back(temp);
	}
}

void ARApp::CleanUp()
{
	smartRelease();
	sampleRelease();

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete input_manager_;
	input_manager_ = NULL;

	delete reconfigureLiveFeed;
	reconfigureLiveFeed = NULL;

	//Clean up loaded models
	delete car;
	car = NULL;

	//Clean up track pieces
	delete track_gameObject;
	for (int i = 0; i < 6; i++)
	{
		track_gameObject[i] = NULL;
	}

	//Clean up textures
	delete shadow_texture;
	shadow_texture = NULL;

	delete road_texture;
	road_texture = NULL;

	delete speedoSprite;
	speedoSprite = NULL;

	delete speedoNeedleSprite;
	speedoNeedleSprite = NULL;
}

bool ARApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	controller_input.Update(frame_time);

	// set the transform of the 3D mesh instance to draw on
	// top of the marker
	marker_transforms = liveCameraFeed.Update();

	//Reset the car and race timer
	if (car_gameObject.GetResetBool())
	{
		ResetTimer();
		car_gameObject.SetResetBool(false);
	}

	if (marker_transforms.size() > 0)
	{
		collision_bool.clear();

		for (std::vector<LiveFeed::MarkerTransforms>::iterator iter = marker_transforms.begin(); iter != marker_transforms.end(); iter++)
		{
			render[iter->markerID] = false; //set relevant render bool to false
			track_gameObject[iter->markerID]->set_transform(iter->transform); //the track pieces can just have their transforms set directly

			car_gameObject.SetTransform(marker_transforms);
			raceGameObject.set_transform(marker_transforms[0].transform); //move the start position to marker 0 if its on screen, otherwise any that is available

			//Update the car gameobject - should only do this if the car is being rendered
			car_gameObject.Update(frame_time);

			//Check collisions
			if (car_gameObject.AABBCollision(*track_gameObject[iter->markerID]))
			{
				//If the car is colliding with the track, switch off gravity
				car_gameObject.SetZVelocity(0.0f);
				collision_bool.push_back(true);
			}

			//If the car has moved off of the starting position, start the race
			if (!car_gameObject.AABBCollision(raceGameObject))
			{
				//raceTimer is used to take get the total elapsed time - useful for checking scoreboard positions
				currentTime.raceTimer += frame_time;
				//Breakdown the timer into milliseconds, seconds and minutes for UI display purposes
				currentTime.raceMilliseconds += frame_time;

				if (currentTime.raceMilliseconds > 1.0f)
				{
					currentTime.raceSeconds++;
					currentTime.raceMilliseconds = 0.0f;
				}

				if (currentTime.raceSeconds > 59.0f)
				{
					currentTime.raceMinutes++;
					currentTime.raceSeconds = 0;
				}
			}
			else
			{
				//If the car crosses the line, reset the timer and update the scoreboard
				ResetTimer();
			}

			//A marker has been found, set the relevant render bool to true
			render[iter->markerID] = true;
		}

		//Marker 0 MUST be on screen, or track can't be built
		if (marker_transforms[0].markerID == 0)
		{
			//Call function here to build track
			BuildTrack(track_gameObject);
		}

		if (collision_bool.size() == 0)
		{
			car_gameObject.SetZVelocity(-0.0981f); //give the car gravity [cm/s^2]
		}
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			render[i] = false;
		}
	}

	return true;
}

void ARApp::Render()
{
	//
	// Render the camera feed
	//

	// REMEMBER AND SET THE PROJECTION MATRIX HERE
	sprite_renderer_->set_projection_matrix(liveCameraFeed.GetOrthoMatrix());

	sprite_renderer_->Begin(true);

	//Be sure to draw the live feed from the camera
	liveCameraFeed.BeginRender();
	sprite_renderer_->DrawSprite(liveCameraFeed.GetCameraSprite());

	sprite_renderer_->End();

	//
	// Render 3D scene
	//

	// SET VIEW AND PROJECTION MATRIX HERE
	renderer_3d_->set_view_matrix(liveCameraFeed.GetViewMatrix());
	renderer_3d_->set_projection_matrix(liveCameraFeed.GetProjectionMatrix());

	// Begin rendering 3D meshes, don't clear the frame buffer
	renderer_3d_->Begin(false);

	// check to see if a particular marker can be found
	if (render[0]) //do this for each track peice - for the car simply do an iterator as it should have a position relative to all markers anyway
	{
		// DRAW 3D MESHES HERE
		//Track
		if (marker_transforms.size() > 0)
		{
			for (std::vector<LiveFeed::MarkerTransforms>::iterator iter = marker_transforms.begin(); iter != marker_transforms.end(); iter++)
			{
				renderer_3d_->DrawMesh(*track_gameObject[iter->markerID]);
			}
		}

		//Car and shadow
		abfw::Matrix44 car_transform = car_gameObject.transform();
		//Offset the car slightly to allow the shadow to sit underneath
		car_transform.SetTranslation(car_gameObject.transform().GetTranslation() + abfw::Vector3(0.0f, 0.0001f, 0.0f));

		//The loaded in model is huge and loads in rotated - fix it before rendering so the rotations don't mess up the controls
		abfw::Matrix44 scale_matrix, rotx_matrix, rotz_matrix;
		scale_matrix.Scale(abfw::Vector3(0.01f, 0.01f, 0.01f));
		rotx_matrix.RotationX(FRAMEWORK_PI / 2.0f);
		rotz_matrix.RotationZ(FRAMEWORK_PI);
	
		//Set the transform with the new scale and rotations
		car_gameObject.set_transform(scale_matrix * rotx_matrix * rotz_matrix * car_transform);
		
		//Position the shadow under the car
		car_transform.SetTranslation(car_gameObject.transform().GetTranslation() - abfw::Vector3(0.0f, 0.000001f, 0.0f));
		scale_matrix.Scale(abfw::Vector3(1.4f, 1.0f, 1.0f));
		car_shadow_gameObject.set_transform(scale_matrix * car_transform);

		renderer_3d_->DrawMesh(car_gameObject);
		renderer_3d_->DrawMesh(car_shadow_gameObject);
	}

	renderer_3d_->End();

	RenderOverlay();
}

void ARApp::RenderOverlay()
{
	//
	// render 2d hud on top
	//
	abfw::Matrix44 proj_matrix2d;
	proj_matrix2d.OrthographicFrustumVita(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);
	sprite_renderer_->Begin(false);

	//Get sprite rotation for speedoNeedle from the current velocity of the car
	float maxRot = 4.0f * FRAMEWORK_PI / 3.0f; //240 degrees - speedo sprite stops at 100mph at 240 degrees
	float currentRot = maxRot * std::abs(car_gameObject.GetVelocityPercentage());
	speedoNeedle->set_rotation(currentRot);

	//Draw the UI
	sprite_renderer_->DrawSprite(*speedoNotches);
	sprite_renderer_->DrawSprite(*speedoNeedle);
	DrawHUD();
	sprite_renderer_->End();
}

void ARApp::InitFont()
{
	font_ = new abfw::Font();
	font_->Load("comic_sans", platform_);
}

void ARApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void ARApp::DrawHUD()
{
	int displacement = 0;
	if(font_)
	{
		//Convert the float counting the milliseconds into an integer that's useful for the UI
		int raceMillisecondsAsInt = (int)std::pow(1000, currentTime.raceMilliseconds) - 1;
		
		//Position and render the font-based UI on screen
		font_->RenderText(sprite_renderer_, abfw::Vector3(400.0f, 10.0f, -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "%.2i :", currentTime.raceMinutes);
		font_->RenderText(sprite_renderer_, abfw::Vector3(450.0f, 10.0f, -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "%.2i :", currentTime.raceSeconds);
		font_->RenderText(sprite_renderer_, abfw::Vector3(500.0f, 10.0f, -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "%.3i", raceMillisecondsAsInt);
		font_->RenderText(sprite_renderer_, abfw::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "FPS: %.1f", fps_);
		font_->RenderText(sprite_renderer_, abfw::Vector3(10.0f, 260.0f, -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "Best Times :");

		//Iterate through the best times and separate the times appropriately
		for (std::vector<Times>::iterator iter = bestTimes.begin(); iter != bestTimes.end(); iter++)
		{
			raceMillisecondsAsInt = (int)std::pow(1000, iter->raceMilliseconds) - 1;
			font_->RenderText(sprite_renderer_, abfw::Vector3(10.0f, 310.0f + (50.0f * displacement), -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "%.2i :", iter->raceMinutes);
			font_->RenderText(sprite_renderer_, abfw::Vector3(60.0f, 310.0f + (50.0f * displacement), -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "%.2i :", iter->raceSeconds);
			font_->RenderText(sprite_renderer_, abfw::Vector3(110.0f, 310.0f + (50.0f * displacement), -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "%.3i", raceMillisecondsAsInt);
			displacement++;
		}
	}
}

abfw::Mesh* ARApp::CreatePlaneMesh(abfw::TextureVita* texture, abfw::Vector3 scale, int planeType)
{
	//The plane mesh is set to the size of the markers by default. 
	//The scale allows for resizing of the objects when necessary.
	abfw::Mesh* mesh = platform_.CreateMesh();
	abfw::Vector2 texCoords[4];

	//The plane types allow the cheap adjustment of the texture coordinates to provide straight roads and corners using the same texture - planeType is set to 1 by default in the header
	switch (planeType)
	{
	case 1: //Normal
		texCoords[0] = { 1.0f, 0.0f }; //bottom right
		texCoords[1] = { 1.0f, 1.0f }; //top right
		texCoords[2] = { 0.0f, 1.0f }; //top left
		texCoords[3] = { 0.0f, 0.0f }; //bottom left
		break;
	case 2: //Corner
		texCoords[0] = { 1.0f, 0.0f };
		texCoords[1] = { 1.0f, 1.0f };
		texCoords[2] = { 1.0f, 1.0f };
		texCoords[3] = { 0.0f, 1.0f };
		break;
	}

	//Set up the vertices for the vertex buffer
	const abfw::Mesh::Vertex vertices[] = {
		{ abfw::Vector3((0.059f / 2.0f) * scale.x, -(0.059f / 2.0f) * scale.y, 0.0f), abfw::Vector3(0.0f, 0.0f, 1.0f), texCoords[0] }, //bottom right
		{ abfw::Vector3((0.059f / 2.0f) * scale.x, (0.059f / 2.0f) * scale.y, 0.0f), abfw::Vector3(0.0f, 0.0f, 1.0f), texCoords[1] },  //top right
		{ abfw::Vector3(-(0.059f / 2.0f) * scale.x, (0.059f / 2.0f) * scale.y, 0.0f), abfw::Vector3(0.0f, 0.0f, 1.0f), texCoords[2] }, //top left
		{ abfw::Vector3(-(0.059f / 2.0f) * scale.x, -(0.059f / 2.0f) * scale.y, 0.0f), abfw::Vector3(0.0f, 0.0f, 1.0f), texCoords[3] } //bottom left
	};

	mesh->InitVertexBuffer(platform_, static_cast<const void*>(vertices), sizeof(vertices) / sizeof(abfw::Mesh::Vertex), sizeof(abfw::Mesh::Vertex));

	mesh->AllocatePrimitives(1);
	abfw::Primitive* primitive = mesh->GetPrimitive(0);

	//Set up the rendering order of the vertices using the index buffer
	const UInt32 indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	primitive->InitIndexBuffer(platform_, static_cast<const void*>(indices), sizeof(indices) / sizeof(UInt32), sizeof(UInt32));
	primitive->SetType(abfw::TRIANGLE_LIST);
	primitive->set_texture(texture);
	
	//Might as well set up collision detection for the plane whilst we're here
	abfw::Aabb aabb(abfw::Vector3(-(0.059f / 2.0f) * scale.x, -(0.059f / 2.0f) * scale.y, 0.0f), abfw::Vector3((0.059f / 2.0f) * scale.x, (0.059f / 2.0f) * scale.y, 0.0f));
	mesh->set_aabb(aabb);

	return mesh;
}

abfw::Mesh* ARApp::CreateCubeMesh(float size) //meters
{
	//The cube can be set to any size the user desires.
	//If they require an alternative to a perfect cube, scaling is a an option. 
	abfw::Mesh* mesh = platform_.CreateMesh();

	//Set up the vertices for the vertex buffer
	const abfw::Mesh::Vertex vertices[] = {
		{ abfw::Vector3(size, -size, -size), abfw::Vector3(0.577f, -0.577f, -0.577f), abfw::Vector2(1.0f, 0.0f) },
		{ abfw::Vector3(size, size, -size), abfw::Vector3(0.577f, 0.577f, -0.577f), abfw::Vector2(1.0f, 1.0f) },
		{ abfw::Vector3(-size, size, -size), abfw::Vector3(-0.577f, 0.577f, -0.577f), abfw::Vector2(0.0f, 1.0f) },
		{ abfw::Vector3(-size, -size, -size), abfw::Vector3(-0.577f, -0.577f, -0.577f), abfw::Vector2(0.0f, 0.0f) },
		{ abfw::Vector3(size, -size, size), abfw::Vector3(0.577f, -0.577f, 0.577f), abfw::Vector2(1.0f, 0.0f) },
		{ abfw::Vector3(size, size, size), abfw::Vector3(0.577f, 0.577f, 0.577f), abfw::Vector2(1.0f, 1.0f) },
		{ abfw::Vector3(-size, size, size), abfw::Vector3(-0.577f, 0.577f, 0.577f), abfw::Vector2(0.0f, 1.0f) },
		{ abfw::Vector3(-size, -size, size), abfw::Vector3(-0.577f, -0.577f, 0.577f), abfw::Vector2(0.0f, 0.0f) } };

	mesh->InitVertexBuffer(platform_, static_cast<const void*>(vertices), sizeof(vertices) / sizeof(abfw::Mesh::Vertex), sizeof(abfw::Mesh::Vertex));

	mesh->AllocatePrimitives(1);
	abfw::Primitive* primitive = mesh->GetPrimitive(0);

	//Set up the rendering order of the vertices using the index buffer
	const UInt32 indices[] = {
		// Back
		0, 1, 2,
		2, 3, 0,
		// Front
		6, 5, 4,
		4, 7, 6,
		// Left
		2, 7, 3,
		2, 6, 7,
		// Right
		0, 4, 1,
		5, 1, 4,
		// Top
		6, 2, 1,
		5, 6, 1,

		// Bottom
		0, 3, 7,
		0, 7, 4
	};

	primitive->InitIndexBuffer(platform_, static_cast<const void*>(indices), sizeof(indices) / sizeof(UInt32), sizeof(UInt32));
	primitive->SetType(abfw::TRIANGLE_LIST);
	
	//Might as well set up collision detection for the cube whilst we're here
	abfw::Aabb aabb(abfw::Vector3(-size, -size, -size), abfw::Vector3(size, size, size));
	mesh->set_aabb(aabb);
	return mesh;
}

void ARApp::BuildTrack(GameObject** trackPieces)
{
	//This function takes in the array pointer for the track pieces loaded into the game. 
	//It centres the track around the first marker, marker 0.
	//By calculating the distance between markers using a similar system to the collision detection, the extra pieces will 'snap' to marker 0 to form a track.
	//Once a side of marker 0 is filled with a track piece, the new piece of track can now have more pieces attached to it if the player desires.
	//Each subsequent track piece can rotated by physically rotating the marker and it will snap to 90 degree rotations to align with the grid.
	
	int x = 0;
	int y = 0;
	float r = 0.2f; // just over standard track piece radius of 0.0885f
	abfw::Vector3 xTranslation = { 0.059f * 3.0f, 0.0f, 0.0f }; //Silly use of magic numbers, but gives exact diameter of each track piece
	abfw::Vector3 yTranslation = { 0.0f, 0.059f * 3.0f, 0.0f };
	abfw::Matrix44 tempIdentityMatrix;
	tempIdentityMatrix.SetIdentity();

	//Clear both vectors and add markers 1 - 5 to notTrack
	notTrack.clear();
	track.clear();

	for (int i = 1; i < 6; i++)
	{
		notTrack.push_back(i);
	}

	//Add marker 0 to track list
	track.push_back({ 0, x, y });

	//Loop over other markers and check if any are within range
	for (int i = 1; i < 6; i++)
	{
		x = y = 0;
		float minDist = 0.0f;

		//Don't do anything with this track piece if it's transform is equal to the identity matrix - it's not on screen and will break everything
		if (trackPieces[i]->transform().GetTranslation().x != tempIdentityMatrix.GetTranslation().x &&
			trackPieces[i]->transform().GetTranslation().y != tempIdentityMatrix.GetTranslation().y &&
			trackPieces[i]->transform().GetTranslation().z != tempIdentityMatrix.GetTranslation().z)
		{
			//Get Euclidean distance between marker positions
			abfw::Vector3 dist = trackPieces[i]->transform().GetTranslation() - trackPieces[0]->transform().GetTranslation();

			float distance = std::sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);

			//If within range, change track piece marker to marker 0 and position accordingly
			if (distance <= r)
			{
				//Decide which axis has the least distance and thus the direction the piece needs to be translated away from marker 0
				if (abs(dist.x) < r)
					minDist = abs(dist.x);

				if (abs(dist.y) > minDist)
				{
					if (dist.y < 0.0f)
						y--;
					else
						y++;
				}
				else
				{
					if (dist.x < 0.0f)
						x--;
					else
						x++;
				}

				for (std::vector<Track>::iterator iter2 = track.begin(); iter2 != track.end(); iter2++)
				{
					//Need to check if a marker already has this x and y position before adding to track -- must be done before erase!
					if (iter2->x != x || iter2->y != y)
					{
						//Remove track piece from notTrack list
						for (std::vector<int>::iterator iter = notTrack.begin(); iter != notTrack.end(); iter++)
						{
							if (*iter == i)
							{
								notTrack.erase(iter);
								break; //break so we don't keep iterating and possibly go out of bounds of the vector
							}
						}

						//Add track piece to track list
						track.push_back({ i, x, y });

						//Get major rotation - 0, 90, 180, 270
						abfw::Matrix44 tempMatrix, tempLocal;
						tempMatrix.SetIdentity();
						tempLocal.SetIdentity();
						trackPieces[i]->RotateForward();

						//Rotate the track piece and translate it into position
						tempLocal.RotationZ(trackPieces[i]->GetMajorRotation());
						tempMatrix = trackPieces[0]->transform();
						tempLocal.SetTranslation((xTranslation * x) + (yTranslation * y));
						trackPieces[i]->set_transform(tempLocal * tempMatrix);
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	
	//Loop over the track pieces that are already attached to marker 0 and check if any of the remaining markers are close by
	for (std::vector<Track>::iterator iterTrack = track.begin(); iterTrack != track.end(); )
	{
		for (std::vector<int>::iterator iterNT = notTrack.begin(); iterNT != notTrack.end(); )
		{
			x = y = 0;

			//Don't bother checking marker 0 or if the remaining track piece is on the origin - it isn't on screen
			if (iterTrack->markerID != 0 && (trackPieces[*iterNT]->transform().GetTranslation().x != tempIdentityMatrix.GetTranslation().x && 
				trackPieces[*iterNT]->transform().GetTranslation().y != tempIdentityMatrix.GetTranslation().y && 
				trackPieces[*iterNT]->transform().GetTranslation().z != tempIdentityMatrix.GetTranslation().z))
			{
				float minDist = 0.0f;

				//Get Euclidean distance between marker positions
				abfw::Vector3 dist = trackPieces[*iterNT]->transform().GetTranslation() - trackPieces[iterTrack->markerID]->transform().GetTranslation();

				float distance = std::sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);

				if (distance <= r)
				{
					//Set the x and y translations (from marker 0) so that the new track piece is effectively attached to them within the grid
					x = iterTrack->x;
					y = iterTrack->y;

					//Decide which axis has the least distance and thus the direction the piece needs to be translated away from marker 0
					if (abs(dist.x) < r)
						minDist = abs(dist.x);

					if (abs(dist.y) > minDist)
					{
						if (dist.y < 0.0f)
							y--;
						else
							y++;
					}
					else
					{
						if (dist.x < 0.0f)
							x--;
						else
							x++;
					}

					//Make sure there's not already a track piece in that grid location
					if (iterTrack->x != x || iterTrack->y != y)
					{
						//Add track piece to track list
						track.push_back({ *iterNT, x, y });

						//Get major rotation - 0, 90, 180, 270
						abfw::Matrix44 tempMatrix, tempLocal;
						tempMatrix.SetIdentity();
						tempLocal.SetIdentity();
						trackPieces[*iterNT]->RotateForward();

						//Rotate the track piece and translate it into position
						tempLocal.RotationZ(trackPieces[*iterNT]->GetMajorRotation());
						tempMatrix = trackPieces[0]->transform();
						tempLocal.SetTranslation((xTranslation * x) + (yTranslation * y));
						trackPieces[*iterNT]->set_transform(tempLocal * tempMatrix);

						//Remove track piece from notTrack list
						for (std::vector<int>::iterator iter = notTrack.begin(); iter != notTrack.end(); iter++)
						{
							if (*iter == *iterNT)
							{
								notTrack.erase(iter);
								break;
							}
						}

						break;
					}
				}
				else
				{
					iterNT++;
				}
			}
			else
			{	
				iterNT++;
			}
		}

		iterTrack++;
	}
}

void ARApp::ResetTimer()
{
	//If the timer is not 0, reset
	if (currentTime.raceTimer != 0.0f)
	{
		//Loop over the current best times and add the new time to the vector if it beats any of the scores
		for (std::vector<Times>::iterator iter = bestTimes.begin(); iter != bestTimes.end(); iter++)
		{
			if (currentTime.raceTimer < iter->raceTimer)
			{
				bestTimes.insert(iter, currentTime);
				break;
			}
		}
	}

	//Make sure vector doesn't get too large - no point in rendering times off the screen
	if (bestTimes.size() > 5)
	{
		for (std::vector<Times>::iterator iter = bestTimes.begin() + 5; iter != bestTimes.end(); ) //Don't increase counter as vector will shift when removing elements
		{
			bestTimes.erase(iter); 
		}
	}

	//Actually reset the timer
	currentTime.raceMilliseconds = 0.0f;
	currentTime.raceSeconds = 0;
	currentTime.raceMinutes = 0;
	currentTime.raceTimer = 0.0f;
}

abfw::TextureVita* ARApp::LoadTexture(abfw::TextureVita* outTexture, const char* fileName)
{
	//This function loads a texure, allocates the necessary memory and passes it out 
	abfw::ImageData imageData;
	png_loader.Load(fileName, platform_, imageData);
	outTexture = new abfw::TextureVita(platform_, imageData);
	return outTexture;
}