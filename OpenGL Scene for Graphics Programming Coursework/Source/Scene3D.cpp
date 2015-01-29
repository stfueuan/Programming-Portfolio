#include "Scene3D.h"

#define GL_CLAMP_TO_EDGE 0x812F //help prevent seams on objects such as the spherical tree (tree2)

void Scene3D::Init(HWND& wnd)
{
	hwnd = wnd;
	GetClientRect(hwnd, &screenRect); //get rect into our handy global rect
	InitializeOpenGL(screenRect.right, screenRect.bottom); // initialise openGL

	srand(time(NULL)); //seed randomiser

	camera.init();
	lighting.init();

	//OpenGL settings
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f); // Black Background
	glClearDepth(1.0f); // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glClearStencil(0);

	//Initialise any variables here
	//for moving light
	angle = 0.0f;
	radius = 20.0f;
	speed = 0.01f;
	fire_height = 6;
	fire_outliers_height = 4;
	shadow_alpha = 0.9f;
	point_light_attenuation = 1.0f;

	//for bird model
	bird_angle = 0;
	bird_angle2 = 0;

	//convert screen coords to window coords for SetCursorPos (which takes screen coordinates)
	point.x = (screenRect.left + screenRect.right)/2;
	point.y = (screenRect.top + screenRect.bottom)/2;

	ClientToScreen(wnd, &point); 

	//initialise textures
	textures();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//tri-linear sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//initialise display lists
	display_list1();
	floor();
	tree();
	log();
	wall();

	//initialise fire variables
	fired.fire_x = fired.fire_y = fired.fire_z = 0;
	fired.list = Dlist;
	fired.is_active = true;
	fired.colour[0] = 1.0f;
	fired.colour[1] = fired.colour[2] = 0.0f;
	fired.light_number = GL_LIGHT0;

	//push cubes into the vector for translation
	for(int fire = 0; fire < 25; fire++)
	{
		fire_cubes.push_back(fired);
	}

	for(int fire = 0; fire < 12; fire++)
	{
		fire_outliers.push_back(fired);
	}

	//load models into respective display lists
	//Found at: http://www.turbosquid.com/FullPreview/Index.cfm/ID/696876 with Royalty-free license
	//Author: TonyCST; Sep 30, 2012
	chicken_model.Initialise("Models/chicken.obj", "Textures/chicken.png");

	//Found at: http://www.turbosquid.com/FullPreview/Index.cfm/ID/644901 with Royalty-free license
	//Author: gripati; Dec 11, 2011
	caveman_model.Initialise("Models/caveman.obj", "Textures/CM_TextureMap.png");

	//Found at: http://www.turbosquid.com/FullPreview/Index.cfm/ID/618348 with Royalty-free license
	//Author: turbomonkey08; Jul 21, 2011
	tree_model.Initialise("Models/Tree.obj", "Textures/Tree_Dry_1_Diffuse2048.png");

	//Found at: http://opengameart.org/content/wolf-0 with Creative Commons license (http://creativecommons.org/licenses/by-sa/3.0/) - no changes to model made
	//Author: Sirren; Jul 10, 2012
	wolf_model.Initialise("Models/wolf.obj", "Textures/pelt.png");

	//Found at: http://opengameart.org/content/bird-animated with Creative Commons license (http://creativecommons.org/licenses/by-sa/3.0/) - no changes to model made
	//Author: PantherOne; Jul 29, 2013
	bird_model.Initialise("Models/bird.obj", "Textures/Bird_0");

	//initialise shadows into respective display lists
	GLfloat Light_Position[] = {0.0f, 2.0f, 0.0f, 1.0f}; //artificial light position for better shadows (simply 1 unit above actual light)
	wolf_shadow.Model_Shadow(Light_Position, wolf_model, Vec3(-6.0f, 0.0f, 7.0f), Vec3(0.1f, 0.1f, 0.1f), Vec3(0.0f, 1.0f, 0.0f), -75.0f, shadow_alpha);
	tree_shadow.Model_Shadow(Light_Position, tree_model, Vec3(0.0f, -0.31f, -6.0f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha);
	tree_shadow2.Model_Shadow(Light_Position, tree_model, Vec3(-2.0f, -0.31f, 6.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 1.0f, 0.0f), 45.0f, shadow_alpha);
	chicken_shadow.Model_Shadow(Light_Position, chicken_model, Vec3(-9.0f, 0.0f, 8.0f), Vec3(0.05f, 0.05f, 0.05f), Vec3(0.0f, 1.0f, 0.0f), 95.0f, shadow_alpha);
	caveman_shadow.Model_Shadow(Light_Position, caveman_model, Vec3(3.0f, 0.0f, 0.0f), Vec3(0.0075f, 0.0075f, 0.0075f), Vec3(0.0f, 1.0f, 0.0f), 180.0f, shadow_alpha);
}

void Scene3D::InitializeOpenGL(int width, int height) 
{ 
	hdc = GetDC(hwnd); // sets global HDC
	if (!SaM.CreatePixelFormat(hdc)) // sets pixel format
		PostQuitMessage (0);
	hrc = wglCreateContext(hdc); // creates rendering context from hdc
	wglMakeCurrent(hdc, hrc); //Use this HRC.
	ResizeGLWindow(width, height); // Setup the Screen
}

void Scene3D::plane()
{
	//http://mathproofs.blogspot.com.au/2005/07/mapping-cube-to-sphere.html
	//Maths for the plane transformation found here on 25 April, 2013
	//Author: Philip Nowell
	for(int i = -10; i < 10; i++)
	{
		for(int j = -10; j < 10; j++)
		{
			for(int k = 10; k <= 10; k++)
			{
				float a = i/10.0f;
				float b = j/10.0f;
				float c = k/10.0f;

				//bend points of the plane to match the curve of a sphere
				float dx = a * sqrtf(1.0f - ((b*b)/2.0f) - ((c*c)/2.0f) + (((b*b)*(c*c))/3.0f));
				float dy = b * sqrtf(1.0f - ((c*c)/2.0f) - ((a*a)/2.0f) + (((c*c)*(a*a))/3.0f));
				float dz = c * sqrtf(1.0f - ((a*a)/2.0f) - ((b*b)/2.0f) + (((a*a)*(b*b))/3.0f));

				float dx2 = (a + 0.1f) * sqrtf(1.0f - ((b*b)/2.0f) - ((c*c)/2.0f) + (((b*b)*(c*c))/3.0f));
				float dy2 = b * sqrtf(1.0f - (c*c/2.0f) - ((a + 0.1f)*(a + 0.1f)/2.0f) + (((c*c)*((a + 0.1f)*(a + 0.1f)))/3.0f));
				float dz2 = c * sqrtf(1.0f - (((a + 0.1f)*(a + 0.1f))/2.0f) - ((b*b)/2.0f) + ((((a + 0.1f)*(a + 0.1f))*(b*b))/3.0f));

				float dx3 = (a + 0.1f) * sqrtf(1.0f - (((b + 0.1f)*(b + 0.1f))/2.0f) - ((c*c)/2.0f) + ((((b + 0.1f)*(b + 0.1f))*(c*c))/3.0f));
				float dy3 = (b + 0.1f) * sqrtf(1.0f - ((c*c)/2.0f) - (((a + 0.1f)*(a + 0.1f))/2.0f) + (((c*c)*((a + 0.1f)*(a + 0.1f)))/3.0f));
				float dz3 = c * sqrtf(1.0f - (((a + 0.1f)*(a + 0.1f))/2.0f) - (((b + 0.1f)*(b + 0.1f))/2.0f) + ((((a + 0.1f)*(a + 0.1f))*((b + 0.1f)*(b + 0.1f)))/3.0f));

				float dx4 = a * sqrtf(1.0f - (((b + 0.1f)*(b + 0.1f))/2.0f) - ((c*c)/2.0f) + ((((b + 0.1f)*(b + 0.1f))*(c*c))/3.0f));
				float dy4 = (b + 0.1f) * sqrtf(1.0f - ((c*c)/2.0f) - ((a*a)/2.0f) + (((c*c)*(a*a))/3.0f));
				float dz4 = c * sqrtf(1.0f - ((a*a)/2.0f) - (((b + 0.1f)*(b + 0.1f))/2.0f) + (((a*a)*((b + 0.1f)*(b + 0.1f)))/3.0f));

				glBegin(GL_QUADS);
					glTexCoord2f((a + 1)/2 + 0.05f, (b + 1)/2);
					glNormal3f(0.0f, 0.0f, 1.0f);
						glVertex3f(dx2, dy2, dz2); //top right

					glTexCoord2f((a + 1)/2, (b + 1)/2);
					glNormal3f(0.0f, 0.0f, 1.0f);
						glVertex3f(dx, dy, dz); //top left

					glTexCoord2f((a + 1)/2, (b + 1)/2 + 0.05f);
					glNormal3f(0.0f, 0.0f, 1.0f);
						glVertex3f(dx4, dy4, dz4); //bottom left

					glTexCoord2f((a + 1)/2 + 0.05f, (b + 1)/2 + 0.05f);
					glNormal3f(0.0f, 0.0f, 1.0f);
						glVertex3f(dx3, dy3, dz3); //bottom right

				glEnd();
			}
		}
	}
}

void Scene3D::skybox()
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, myTexture);
	glTranslatef(camera.position.x, camera.position.y, camera.position.z);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
		//behind you
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, sky_back);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glRotatef(180, 0, 0, 1);
				plane();
		glPopMatrix();

		//above you
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, sky_top);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glRotatef(-90, 1, 0, 0);
					plane();
		glPopMatrix();

		//below you
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, sky_bottom);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glRotatef(90, 1, 0, 0);
					plane();
		glPopMatrix();

		//left of you
		glPushMatrix();
			glRotatef(-90, 0, 1, 0);
				glBindTexture(GL_TEXTURE_2D, sky_left);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glRotatef(180, 0, 0, 1);
						plane();
		glPopMatrix();

		//right of you
		glPushMatrix();
			glRotatef(90, 0, 1, 0);
				glBindTexture(GL_TEXTURE_2D, sky_right);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glRotatef(180, 0, 0, 1);
						plane();
		glPopMatrix();

		//in front of you
		glPushMatrix();
			glRotatef(180, 0, 1, 0);
				glBindTexture(GL_TEXTURE_2D, sky_front);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glRotatef(180, 0, 0, 1);
						plane();
		glPopMatrix();

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void Scene3D::wall()
{
	wall_dlist = glGenLists(1);

	glNewList(wall_dlist, GL_COMPILE);

		glScalef(20.0f, 20.0f, 20.0f);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-0.5f, 1.0f, 0.0f); //top left
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
				glVertex3f(0.5f, 1.0f, 0.0f); //top right
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
				glVertex3f(0.5f, 0.0f, 0.0f); //bottom right
			glNormal3f(0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-0.5f, 0.0f, 0.0f); //bottom left
		glEnd();

	glEndList();
}

void Scene3D::ResizeGLWindow(int width, int height) // Initialize and size the GL Window
{
	if (height == 0) // Prevent A Divide By Zero error
	{
		height = 1; // Make the Height Equal One
	}
	
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//calculate aspect ratio
	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1 ,150.0f);
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix
}

void Scene3D::DrawScene(Input& input) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //needed for shadow/mirror stencil buffer

	// load Identity Matrix
	glLoadIdentity();
	
	camera_func(input);

	skybox();

	glDisable(GL_BLEND);
		room();
	glEnable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

		shadows();

		//user controlled fire
		fire_input(input);

		//place and set up fire
		for (int fire_num_x = -2; fire_num_x < 3; fire_num_x++)
		{
			for (int fire_num_z = -2; fire_num_z < 3; fire_num_z++)
			{		
				int fire_height_rand = rand()%fire_height;
				float fire_position[] = {fire_num_x, fire_num_z};
				fire(fire_cubes.size(), fire_height_rand, fire_position);
			}
		}
	
		//put all the parts of the fire together
		for (int fire_num = -1; fire_num < 2; fire_num++)
		{
			int fire_height_rand = rand()%fire_outliers_height;
			fire_construction(fire_num, fire_height_rand);
		}

	glEnable(GL_TEXTURE_2D);

	//put all the objects in place
	objects();

	if(input.isKeyDown(VK_SPACE))
	{
		wired = !wired;
		wire();

		input.SetKeyUp(VK_SPACE);
	}

	// Swap the frame buffers.
	SwapBuffers(hdc);
}

void Scene3D::Resize(HWND& wnd)
{
	hwnd = wnd;
	GetClientRect(hwnd, &screenRect);
	ResizeGLWindow(screenRect.right, screenRect.bottom);
}

void Scene3D::wire()
{
	//are we in wireframe mode?
	if(wired == true)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
	}
	else 
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}
}

void Scene3D::display_list1()
{
	Dlist = glGenLists(1);

	glNewList(Dlist, GL_COMPILE);

		Draw_CUBE(); //Vertex arrays woo!

	glEndList();
}

void Scene3D::floor()
{
	floor_dlist = glGenLists(1);

	glNewList(floor_dlist, GL_COMPILE);
	glPushMatrix();

	float tex_coord_u = 0;
	float tex_coord_v = 0;
	float tex_coord_offset = 1.0f/200.0f;

	for(int quad_x = -100; quad_x < 100; quad_x++)
	{
		for(int quad_z = -100; quad_z < 100; quad_z++)
		{
			if (quad_z == 99)
			{
				tex_coord_v = 0;
			}

			glBegin(GL_TRIANGLE_STRIP);
			
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(tex_coord_u, tex_coord_v + tex_coord_offset);
					glVertex3f(quad_x, 0.0f, quad_z + 1); //bottom left
					
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(tex_coord_u + tex_coord_offset, tex_coord_v + tex_coord_offset);
					glVertex3f(quad_x + 1, 0.0f, quad_z + 1); //bottom right

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(tex_coord_u, tex_coord_v);
					glVertex3f(quad_x, 0.0f, quad_z); //top left

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(tex_coord_u + tex_coord_offset, tex_coord_v);
					glVertex3f(quad_x + 1, 0.0f, quad_z); //top right

			glEnd();	
			
			tex_coord_v += tex_coord_offset;
		}

		tex_coord_u += tex_coord_offset;
	}

	glPopMatrix();
	glEndList();
}

void Scene3D::camera_func(Input& input)
{
	//lock the mouse to the centre of the window
	SetCursorPos(point.x, point.y);

	static int mouse_centre_x = input.setMouseX((screenRect.left + screenRect.right)/2);
	static int mouse_centre_y = input.setMouseY((screenRect.top + screenRect.bottom)/2);

	int mouse_x_offset = input.getMouseX() - mouse_centre_x;
	int mouse_y_offset = input.getMouseY() - mouse_centre_y;

	if(mouse_x_offset < 0)
	{
		camera.ReduceYaw();
		input.setMouseX(mouse_centre_x);
		mouse_x_offset = 0;
	}
	else if(mouse_x_offset > 0)
	{
		camera.AddYaw();
		input.setMouseX(mouse_centre_x);
		mouse_x_offset = 0;
	}

	if(mouse_y_offset < 0)
	{
		camera.AddPitch();
		input.setMouseX(mouse_centre_y);
		mouse_y_offset = 0;
	}
	else if(mouse_y_offset > 0)
	{
		camera.ReducePitch();
		input.setMouseX(mouse_centre_y);
		mouse_y_offset = 0;
	}

	if(input.isKeyDown('W'))
	{
		camera.position.x = (camera.position.x + (camera.forward.x*0.5f));
		camera.position.y = (camera.position.y + (camera.forward.y*0.5f));
		camera.position.z = (camera.position.z + (camera.forward.z*0.5f));
	}
	
	if(input.isKeyDown('D'))
	{
		camera.position.x = (camera.position.x - camera.right.x*0.5f);
		camera.position.y = (camera.position.y - camera.right.y*0.5f);
		camera.position.z = (camera.position.z - camera.right.z*0.5f);
	}

	if(input.isKeyDown('A'))
	{
		camera.position.x = (camera.position.x + camera.right.x*0.5f);
		camera.position.y = (camera.position.y + camera.right.y*0.5f);
		camera.position.z = (camera.position.z + camera.right.z*0.5f);
	}
	
	if(input.isKeyDown('S'))
	{
		camera.position.x = (camera.position.x - (camera.forward.x*0.5f));
		camera.position.y = (camera.position.y - (camera.forward.y*0.5f));
		camera.position.z = (camera.position.z - (camera.forward.z*0.5f));
	}

	if(input.isKeyDown('E'))
	{
		camera.AddRoll();
	}
	else if(input.isKeyDown('Q'))
	{
		camera.ReduceRoll();
	}

	//set camera looking down the -z axis, 6 units away from the centre
	//Where we are, What we look at, and which way is up
	camera.CameraUpdate();
	gluLookAt(camera.position.x, camera.position.y, camera.position.z, (camera.position.x + camera.forward.x), (camera.position.y + camera.forward.y), (camera.position.z + camera.forward.z), camera.up.x, camera.up.y, camera.up.z); 
}

void Scene3D::room()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//ground
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, large_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glScalef(-1/10.0f, 1.0f, -1/10.0f);
			glCallList(floor_dlist);
	glPopMatrix();

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, myTexture);
		glEnable(GL_BLEND); //to allow for the transparency
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//back wall
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 10.0f);
				glCallList(wall_dlist);
		glPopMatrix();

		//front wall
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, myTexture);
			glTranslatef(0.0f, 0.0f, -10.0f);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
				glCallList(wall_dlist);
		glPopMatrix();

		//left wall
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, myTexture);
			glTranslatef(-10.0f, 0.0f, 0.0f);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
				glCallList(wall_dlist);
		glPopMatrix();

		//right wall
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, myTexture);
			glTranslatef(10.0f, 0.0f, 0.0f);
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
				glCallList(wall_dlist);
		glPopMatrix();

		glDisable(GL_BLEND);
	glPopMatrix();
}

void Scene3D::textures()
{
	//This texture was created by combining the following Royalty-free stock images
	//Found at: http://www.freestockphotos.biz/stockphoto/15125
	//Author: Chrisdesign; no date
	//Found at: http://www.freestockphotos.biz/stockphoto/15119
	//Author: Chrisdesign; no date
	//Found at: http://www.freestockphotos.biz/stockphoto/15117
	//Author: Chrisdesign; no date
	//Found at: http://www.freestockphotos.biz/stockphoto/15115
	//Author: Chrisdesign; no date
	//Found at: http://www.freestockphotos.biz/stockphoto/15118
	//Author: Chrisdesign; no date
	//Found at: http://www.freestockphotos.biz/stockphoto/15120
	//Author: Chrisdesign; no date
	myTexture = SOIL_load_OGL_texture
	(
		"Textures/treeline.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	//Found at: http://uttexture.com/UT/Website/TextureDatabase/UT/GenEarth/GenEarth_GroundMain.htm
	//Created by: Brecht a.k.a. DÏËH·rdªß«Wß; no date
	large_texture = SOIL_load_OGL_texture
	(
		"Textures/Dirt_su2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	//all skybox textures were created using Spacescape by Alex C. Peterson
	//http://alexcpeterson.com/spacescape
	sky_left = SOIL_load_OGL_texture
	(
		"Textures/test1_left2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	sky_right = SOIL_load_OGL_texture
	(
		"Textures/test1_right1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	sky_top = SOIL_load_OGL_texture
	(
		"Textures/test1_top3.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	sky_bottom = SOIL_load_OGL_texture
	(
		"Textures/test1_bottom4.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	sky_front = SOIL_load_OGL_texture
	(
		"Textures/test1_front5.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	sky_back = SOIL_load_OGL_texture
	(
		"Textures/test1_back6.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	//Found at: http://www.spiralgraphics.biz/packs/plant_bush_tree/?11
	//No original author, royalty free
	tree_texture = SOIL_load_OGL_texture
	(
		"Textures/tree.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	//Found at: http://kirkdunne.com/blog/?attachment_id=233
	//Created by: Kirk Dunne; June 23, 2011
	bark_texture = SOIL_load_OGL_texture
	(
		"Textures/bark.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
}

void Scene3D::fire(int fire_amount, int fire_height, float fire_pos[])
{
	//randomly select a cube to translate
	int fire_rand = rand()%fire_amount;

	//give light some initial values
	GLfloat Light_position[] = {0.0f, 1.0f, 0.0f, 1.0f};
	GLfloat Specular_light[] = {1.0f, 1.0f, 1.0f};
	GLfloat Diffuse_light[3];

	//randomise cube and light colour
	if (fire_rand <= 1)
	{
		fire_cubes[fire_rand].colour[0] = 1.0f;
		fire_cubes[fire_rand].colour[1] = fire_cubes[fire_rand].colour[2] = 0.0f;

		Diffuse_light[0] = 1.0f;
		Diffuse_light[1] = 0.0f;
		Diffuse_light[2] = 0.0f;
	}
	else if (fire_rand <= 3)
	{
		fire_cubes[fire_rand].colour[0] = 1.0f;
		fire_cubes[fire_rand].colour[1] = 1.0f;
		fire_cubes[fire_rand].colour[2] = 0.0f;

		Diffuse_light[0] = 1.0f;
		Diffuse_light[1] = 1.0f;
		Diffuse_light[2] = 0.0f;
	}
	else
	{
		fire_cubes[fire_rand].colour[0] = 1.0f;
		fire_cubes[fire_rand].colour[1] = 0.549f;
		fire_cubes[fire_rand].colour[2] = 0.0f;
			
		Diffuse_light[0] = 1.0f;
		Diffuse_light[1] = 0.549f;
		Diffuse_light[2] = 0.0f;
	}

	//set up point light
	lighting.point_light(GL_LIGHT1, Light_position, Diffuse_light, Specular_light, point_light_attenuation);

	glPushMatrix();
		glColor3f(fire_cubes[fire_rand].colour[0], fire_cubes[fire_rand].colour[1], fire_cubes[fire_rand].colour[2]);
		glScalef(0.1f, 0.1f, 0.1f);
		glTranslatef(fire_cubes[fire_rand].fire_x + fire_pos[0], fire_cubes[fire_rand].fire_y, fire_cubes[fire_rand].fire_z + fire_pos[1]);

				//put first cubes into position
				for (int i = 0; i < fire_cubes.size(); i++)
				{
					glDisable(GL_LIGHTING); 
						glCallList(fire_cubes[fire_rand].list);
					glEnable(GL_LIGHTING);
				}

			glTranslatef(fire_cubes[fire_rand].fire_x, fire_cubes[fire_rand].fire_y + fire_height, fire_cubes[fire_rand].fire_z);

			//draw cubes to be translated
			glDisable(GL_LIGHTING);
				glCallList(fire_cubes[fire_rand].list);
			glEnable(GL_LIGHTING);
		glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void Scene3D::fire_construction(int fire_number, int fire_height)
{
	//put all elements of the fire together
	int fire_rand = rand()%3;

	glPushMatrix();
		glScalef(0.1f, 0.1f, 0.1f);
		glTranslatef(3.0f, 0.0f, 0.0f);
		glColor3f(fire_cubes[fire_rand].colour[0], fire_cubes[fire_rand].colour[1], fire_cubes[fire_rand].colour[2]);
		glTranslatef(fire_outliers[fire_number + 1].fire_x, fire_outliers[fire_number + 1].fire_y, fire_outliers[fire_number + 1].fire_z + fire_number);
					
			glDisable(GL_LIGHTING);
				glCallList(fire_outliers[fire_number + 1].list);
			glEnable(GL_LIGHTING);
	
			glTranslatef(fire_outliers[fire_rand].fire_x, fire_outliers[fire_rand].fire_y + fire_height, fire_outliers[fire_rand].fire_z);
					
			glDisable(GL_LIGHTING);
				glCallList(fire_outliers[fire_rand].list);
			glEnable(GL_LIGHTING);
		glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
		int fire_rand2 = fire_rand + 3;
		glScalef(0.1f, 0.1f, 0.1f);
		glTranslatef(-6.0f, 0.0f, 0.0f);
			glPushMatrix();
				glTranslatef(3.0f, 0.0f, 0.0f);
				glColor3f(fire_cubes[fire_rand].colour[0], fire_cubes[fire_rand].colour[1], fire_cubes[fire_rand].colour[2]);

					glTranslatef(fire_outliers[fire_number + 4].fire_x, fire_outliers[fire_number + 4].fire_y, fire_outliers[fire_number + 4].fire_z + fire_number);
					
					glDisable(GL_LIGHTING);
						glCallList(fire_outliers[fire_number + 4].list);
					glEnable(GL_LIGHTING);

					glTranslatef(fire_outliers[fire_rand2].fire_x, fire_outliers[fire_rand2].fire_y + fire_height, fire_outliers[fire_rand2].fire_z);
					
					glDisable(GL_LIGHTING);
						glCallList(fire_outliers[fire_rand2].list);
					glEnable(GL_LIGHTING);
				glColor3f(1.0f, 1.0f, 1.0f);
			glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		int fire_rand3 = fire_rand + 6;
		glScalef(0.1f, 0.1f, 0.1f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			glPushMatrix();
			glColor3f(fire_cubes[fire_rand].colour[0], fire_cubes[fire_rand].colour[1], fire_cubes[fire_rand].colour[2]);
				glTranslatef(3.0f, 0.0f, 0.0f);

					glTranslatef(fire_outliers[fire_number + 7].fire_x, fire_outliers[fire_number + 7].fire_y, fire_outliers[fire_number + 7].fire_z + fire_number);
					
					glDisable(GL_LIGHTING);
						glCallList(fire_outliers[fire_number + 7].list);
					glEnable(GL_LIGHTING);

					glTranslatef(fire_outliers[fire_rand3].fire_x, fire_outliers[fire_rand3].fire_y + fire_height, fire_outliers[fire_rand3].fire_z);
					
					glDisable(GL_LIGHTING);
						glCallList(fire_outliers[fire_rand3].list);
					glEnable(GL_LIGHTING);
				glColor3f(1.0f, 1.0f, 1.0f);
			glPopMatrix();
	glPopMatrix();

	glPushMatrix();
		int fire_rand4 = fire_rand + 9;
		glScalef(0.1f, 0.1f, 0.1f);
		glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
			glPushMatrix();
				glColor3f(fire_cubes[fire_rand].colour[0], fire_cubes[fire_rand].colour[1], fire_cubes[fire_rand].colour[2]);
				glTranslatef(3.0f, 0.0f, 0.0f);

					glTranslatef(fire_outliers[fire_number + 10].fire_x, fire_outliers[fire_number + 10].fire_y, fire_outliers[fire_number + 10].fire_z + fire_number);
					
					glDisable(GL_LIGHTING);
						glCallList(fire_outliers[fire_number + 10].list);
					glEnable(GL_LIGHTING);

					glTranslatef(fire_outliers[fire_rand4].fire_x, fire_outliers[fire_rand4].fire_y + fire_height, fire_outliers[fire_rand4].fire_z);
					
					glDisable(GL_LIGHTING);
						glCallList(fire_outliers[fire_rand4].list);
					glEnable(GL_LIGHTING);
				glColor3f(1.0f, 1.0f, 1.0f);
			glPopMatrix();
	glPopMatrix();
}

void Scene3D::fire_input(Input& input)
{
	//if 'z' is pushed, raise the fire; brighten the light and make shadows darker
	//if 'x' is pushed, do the opposite
	if(input.isKeyDown('Z'))
	{
		fire_height += 1;
		fire_outliers_height += 1;
		shadow_alpha += 0.1f;

		if(point_light_attenuation > 1.0f)
		{
			point_light_attenuation -= 1.0f;
		}
		else
		{
			point_light_attenuation -= 0.1f;
		}

		if(fire_height > 10)
		{
			fire_height = 10;
		}

		if(fire_outliers_height > 8)
		{
			fire_outliers_height = 8;
		}

		if(shadow_alpha > 1.0f)
		{
			shadow_alpha = 1.0f;
		}

		if(point_light_attenuation < 0.8f)
		{
			point_light_attenuation = 0.8f;
		}

		//reinitialise model shadow display_lists if alpha changes
		GLfloat Light_Position[] = {0.0f, 2.0f, 0.0f, 1.0f}; //artificial light position for better shadows (simply 1 unit above actual light)
		wolf_shadow.Model_Shadow(Light_Position, wolf_model, Vec3(-6.0f, 0.0f, 7.0f), Vec3(0.1f, 0.1f, 0.1f), Vec3(0.0f, 1.0f, 0.0f), -75.0f, shadow_alpha);
		tree_shadow.Model_Shadow(Light_Position, tree_model, Vec3(0.0f, -0.31f, -6.0f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha);
		tree_shadow2.Model_Shadow(Light_Position, tree_model, Vec3(-2.0f, -0.31f, 6.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 1.0f, 0.0f), 45.0f, shadow_alpha);
		chicken_shadow.Model_Shadow(Light_Position, chicken_model, Vec3(-9.0f, 0.0f, 8.0f), Vec3(0.05f, 0.05f, 0.05f), Vec3(0.0f, 1.0f, 0.0f), 95.0f, shadow_alpha);
		caveman_shadow.Model_Shadow(Light_Position, caveman_model, Vec3(3.0f, 0.0f, 0.0f), Vec3(0.0075f, 0.0075f, 0.0075f), Vec3(0.0f, 1.0f, 0.0f), 180.0f, shadow_alpha);
	}
	else if(input.isKeyDown('X'))
	{
		fire_height -= 1;
		fire_outliers_height -= 1;
		shadow_alpha -= 0.1f;

		if(point_light_attenuation < 1.0f)
		{
			point_light_attenuation += 1.0f;
		}
		else
		{
			point_light_attenuation += 1.0f;
		}

		if(fire_height < 1)
		{
			fire_height = 1;
		}

		if(fire_outliers_height < 1)
		{
			fire_outliers_height = 1;
		}

		if(shadow_alpha < 0.0f)
		{
			shadow_alpha = 0.0f;
		}

		if(point_light_attenuation > 10.0f)
		{
			point_light_attenuation = 10.0f;
		}

		//reinitialise model shadow display_lists if alpha changes
		GLfloat Light_Position[] = {0.0f, 2.0f, 0.0f, 1.0f}; //artificial light position for better shadows (simply 1 unit above actual light)
		wolf_shadow.Model_Shadow(Light_Position, wolf_model, Vec3(-6.0f, 0.0f, 7.0f), Vec3(0.1f, 0.1f, 0.1f), Vec3(0.0f, 1.0f, 0.0f), -75.0f, shadow_alpha);
		tree_shadow.Model_Shadow(Light_Position, tree_model, Vec3(0.0f, -0.31f, -6.0f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha);
		tree_shadow2.Model_Shadow(Light_Position, tree_model, Vec3(-2.0f, -0.31f, 6.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(0.0f, 1.0f, 0.0f), 45.0f, shadow_alpha);
		chicken_shadow.Model_Shadow(Light_Position, chicken_model, Vec3(-9.0f, 0.0f, 8.0f), Vec3(0.05f, 0.05f, 0.05f), Vec3(0.0f, 1.0f, 0.0f), 95.0f, shadow_alpha);
		caveman_shadow.Model_Shadow(Light_Position, caveman_model, Vec3(3.0f, 0.0f, 0.0f), Vec3(0.0075f, 0.0075f, 0.0075f), Vec3(0.0f, 1.0f, 0.0f), 180.0f, shadow_alpha);
	}
}

void Scene3D::tree()
{
	tree_dlist = glGenLists(1);

	glNewList(tree_dlist, GL_COMPILE);

		//tree trunk
		glBindTexture(GL_TEXTURE_2D, bark_texture);
		Draw_CYLINDER(2.0f, 0.5f);

		//pine top
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, tree_texture);
			glTranslatef(0.0f, 1.0f, 0.0f);
				Draw_CONES(3.0f, 1.0f);

			glPushMatrix();
				glTranslatef(0.0f, 1.0f, 0.0f);
					Draw_CONES(3.0f, 1.0f);
			glPopMatrix();
		glPopMatrix();

	glEndList();
}

void Scene3D::tree2()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//bird rotation
	bird_x = cosf(bird_angle)*radius;
	bird_z = sinf(bird_angle)*radius;
	bird_angle += speed;

	bird_angle2 = 180 + bird_x/radius*90.0f;

	//draw trunk
	glBindTexture(GL_TEXTURE_2D, bark_texture);
	Draw_CYLINDER(2.0f, 0.5f);

	//draw tree top
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tree_texture);
		glTranslatef(0.0f, 2.5f, 0.0f);
		glScalef(1.5f, 1.5f, 1.5f);
			Draw_SPHERE();

		//small top sphere
		glPushMatrix();
			glTranslatef(0.0f, 1.0f, 0.0f);
			glScalef(0.2f, 0.2f, 0.2f);
				Draw_SPHERE();

			//even smaller top sphere
			glPushMatrix();
				glTranslatef(0.0f, 1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();

				glPushMatrix();
					glTranslatef(bird_x/10.0f, 0.0f, bird_z/10.0f);
					glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
					glRotatef(bird_angle2, 0.0f, 1.0f, 0.0f);
						glCallList(bird_model.model_dlist);
				glPopMatrix();
			glPopMatrix();

			//must reset the texture after altering it with the bird
			glBindTexture(GL_TEXTURE_2D, tree_texture);

			glPushMatrix();
				glTranslatef(1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(-1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, 1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, -1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();
		glPopMatrix();

		//small sphere at back
		glPushMatrix();
			glTranslatef(1.0f, 0.0f, 0.0f);
			glScalef(0.2f, 0.2f, 0.2f);
				Draw_SPHERE();

			//even smaller top sphere
			glPushMatrix();
				glTranslatef(0.0f, 1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, -1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, 1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, -1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();
		glPopMatrix();

		//small sphere at front
		glPushMatrix();
			glTranslatef(-1.0f, 0.0f, 0.0f);
			glScalef(0.2f, 0.2f, 0.2f);
				Draw_SPHERE();

			//even smaller top sphere
			glPushMatrix();
				glTranslatef(0.0f, 1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, -1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(-1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, 1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, -1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();
		glPopMatrix();

		//small sphere on right
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 1.0f);
			glScalef(0.2f, 0.2f, 0.2f);
				Draw_SPHERE();

			//even smaller top sphere
			glPushMatrix();
				glTranslatef(0.0f, 1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(-1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, 1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, -1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();
		glPopMatrix();

		//small sphere on left
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, -1.0f);
			glScalef(0.2f, 0.2f, 0.2f);
				Draw_SPHERE();

			//even smaller top sphere
			glPushMatrix();
				glTranslatef(0.0f, 1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(-1.0f, 0.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, -1.0f, 0.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(0.0f, 0.0f, -1.0f);
				glScalef(0.2f, 0.2f, 0.2f);
					Draw_SPHERE();
			glPopMatrix();
		glPopMatrix();
			
	glPopMatrix();
}

void Scene3D::log()
{
	log_dlist = glGenLists(1);

	glNewList(log_dlist, GL_COMPILE);

	glPushMatrix();
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
			Draw_CYLINDER(3.0f, 0.5f);
	glPopMatrix();

	glEndList();
}

void Scene3D::shadows()
{
	//Secondary, moving light
	moving_light_x = cosf(angle)*radius;
	moving_light_z = sinf(angle)*radius;
	angle += speed;

	GLfloat Light_Pos[] = {moving_light_x, 10.0f, moving_light_z, 1.0f};
	GLfloat Light_Diff[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat Light_Spec[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat Light_Direction[] = {-moving_light_x, -10.0f, -moving_light_z};
	
	//set up spot light
	lighting.spot_light(GL_LIGHT7, Light_Pos, Light_Diff, Light_Spec, Light_Direction);

	GLfloat Light_Position[] = {0.0f, 2.0f, 0.0f, 1.0f}; //artificial light position for better shadows (simply 1 unit above actual light)

	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

	// draw stencil pattern
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF

 	//ground
	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, large_texture);
		glScalef(1/10.0f, 1.0f, 1/10.0f);
			glCallList(floor_dlist);
	glPopMatrix();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilMask(0x00);
	// draw where stencil's value is 0
	glStencilFunc(GL_EQUAL, 0, 0xFF);
	// draw only where stencil's value is 1
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	glPushMatrix();
		//render shadows
		SaM.Object_Shadow(Light_Position, tree_dlist, Vec3(4.0f, 0.0f, 4.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha);
		SaM.Object_Shadow(Light_Position, tree_dlist, Vec3(7.0f, 0.0f, -7.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha);
		SaM.Object_Shadow(Light_Position, tree_dlist, Vec3(-8.0f, 0.0f, -3.5f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha);
		SaM.Object_Shadow(Light_Position, log_dlist, Vec3(2.0f, 0.25f, 2.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha);
		SaM.Object_Shadow(Light_Position, log_dlist, Vec3(1.0f, 0.25f, -2.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), 40.0f, shadow_alpha);
	glPopMatrix();

	glPushMatrix();
		glCallList(tree_shadow.shadow_dlist);
		glCallList(tree_shadow2.shadow_dlist);
		glCallList(caveman_shadow.shadow_dlist);
		glCallList(wolf_shadow.shadow_dlist);
		glCallList(chicken_shadow.shadow_dlist);
	glPopMatrix();

	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, myTexture);
				SaM.Object_Shadow(Light_Pos, wall_dlist, Vec3(0.0f, 0.0f, 10.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 0.0f, 0.0f), 0.0f, shadow_alpha); //back wall
				SaM.Object_Shadow(Light_Pos, wall_dlist, Vec3(0.0f, 0.0f, 10.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), 180.0f, shadow_alpha); //front wall
				SaM.Object_Shadow(Light_Pos, wall_dlist, Vec3(0.0f, 0.0f, -10.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), -90.0f, shadow_alpha); //left wall
				SaM.Object_Shadow(Light_Pos, wall_dlist, Vec3(0.0f, 0.0f, -10.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f), 90.0f, shadow_alpha); //right wall - only one that works properly
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glDisable(GL_STENCIL_TEST);
}

void Scene3D::objects()
{
	//mirror
	glPushMatrix();
		glTranslatef(-7.0f, 0.0f, 7.5f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			SaM.mirror(wolf_model, floor_dlist, Vec3(-0.0f, 0.0f, 10.0f), Vec3(0.1f, 0.1f, 0.1f), Vec3(0.0f, 1.0f, 0.0f), -195.0f);
	glPopMatrix();

	//procedural trees
	glPushMatrix();
		glTranslatef(4.0f, 0.0f, 4.0f);
			glCallList(tree_dlist);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(7.0f, 0.0f, -7.0f);
			glCallList(tree_dlist);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-8.0f, 0.0f, -3.5f);
			glCallList(tree_dlist);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(7.0f, 0.0f, -1.0f);
			tree2();
	glPopMatrix();

	//logs by the fire
	glPushMatrix();
		glTranslatef(2.0f, 0.25f, 2.0f);
		glBindTexture(GL_TEXTURE_2D, bark_texture);
			glCallList(log_dlist);
	glPopMatrix();

	glPushMatrix();
		glRotatef(40.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(1.0f, 0.25f, -2.0f);
			glCallList(log_dlist);
	glPopMatrix();

	//models
	glPushMatrix();
		glTranslatef(0.0f, -0.31f, -6.0f);
		glScalef(0.5f, 0.5f, 0.5f);
			glCallList(tree_model.model_dlist);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-2.0f, -0.31f, 6.5f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.5f, 0.5f, 0.5f);
			glCallList(tree_model.model_dlist);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-9.0f, 0.0f, 8.0f);
		glRotatef(95.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.05f, 0.05f, 0.05f);
			glCallList(chicken_model.model_dlist);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-6.0f, 0.0f, 7.0f);
		glRotatef(-75.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.1f, 0.1f, 0.1f);
			glCallList(wolf_model.model_dlist);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(3.0f, 0.0f, 0.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.0075f, 0.0075f, 0.0075f);
			glCallList(caveman_model.model_dlist);
	glPopMatrix();
}

