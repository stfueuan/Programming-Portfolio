#include "sprite_application.h"
#include <graphics/sprite_renderer.h>
#include <system/platform.h>
#include <iostream>
#include <math.h>

using namespace std;

SpriteApplication::SpriteApplication(abfw::Platform* platform) :
	abfw::Application(platform),
	sprite_renderer_(NULL),
	player_texture(NULL),
	enemy_texture(NULL),

controller_manager_(SCE_CTRL_MODE_DIGITALANALOG)
{
}

SpriteApplication::~SpriteApplication()
{
}

void SpriteApplication::Init()
{
	// load the font to draw the on-screen text
	bool font_loaded = font_.Load("comic_sans", *platform_);
	if(!font_loaded)
	{
		std::cout << "Font failed to load." << std::endl;
		exit(-1);
	}

	//player texture
	png_loader.Load("tmnt.png", *platform_, image_data);
	player_texture = platform_->CreateTexture(image_data);

	//enemy texture
	png_loader.Load("Shredder.png", *platform_, image_data);
	enemy_texture = platform_->CreateTexture(image_data);

	//background image
	png_loader.Load("Batman.png", *platform_, image_data);
	background_texture = platform_->CreateTexture(image_data);

	// create a sprite renderer to draw the sprites
	sprite_renderer_ = platform_->CreateSpriteRenderer();

	//
	// initialise the sprite properties
	//
	player.set_height(488.0f);
	player.set_width(356.0f);
	player.set_position(abfw::Vector3(250.0f, 175.0f, 0.0f));
	player.set_texture(player_texture);
	player.set_uv_width(0.08f);
	player.set_uv_height(0.143f);
	player.init();

	enemy.push_back(Enemy());
	enemy.push_back(Enemy());
	enemy.push_back(Enemy());

	for(int i = 0; i <enemy.size(); i++)
	{		
		enemy[i].set_position(abfw::Vector3(1000.0+(i*400.0), 300.0f, 0.0f));
		enemy[i].set_height(305.0f);
		enemy[i].set_width(332.5f);
		enemy[i].set_texture(enemy_texture);
		enemy[i].init();		
	}

	//used for background image
	game_object_.set_height(544);
	game_object_.set_width(1920);
	game_object_.set_position(abfw::Vector3(platform_->width(), platform_->height()*0.5f, 0.0f));
	game_object_.set_texture(background_texture);

	game_object_2.set_height(544);
	game_object_2.set_width(1920);
	game_object_2.set_position(abfw::Vector3(-(Int32)platform_->width(), platform_->height()*0.5f, 0.0f));
	game_object_2.set_texture(background_texture);

	score = 0;
}

void SpriteApplication::CleanUp()
{
	// free up the sprite renderer
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete player_texture;
	player_texture = NULL;

	delete enemy_texture;
	enemy_texture = NULL;

	delete background_texture;
	enemy_texture = NULL;
}

bool SpriteApplication::Update(float ticks)
{
	while(!player.dead)
	{
		// calculate the frame rate
		frame_rate_ = 1.0f / ticks;
		srand(time(NULL));
		controller_manager_.Update();

		Collide();
		
		if(player.state != player.hurt)
		{
			if((controller_manager_.buttons_pressed() & (SCE_CTRL_SQUARE | SCE_CTRL_TRIANGLE | SCE_CTRL_CIRCLE | SCE_CTRL_CROSS))>0) 
			{
				//reset player animations
				player.texture_x = 0.0f;
			}
			else if(controller_manager_.buttons_down() & SCE_CTRL_SQUARE) 
			{
				if (player.state == player.idle)
				{
					player.state = player.attack;

					for(int i = 0; i < enemy.size(); i++)
					{
						enemy[i].react();
						score += 10;
					}
				}
			}
			else if(controller_manager_.buttons_down() & SCE_CTRL_TRIANGLE)
			{
				if (player.state == player.idle)
				{
					player.state = player.attack2;

					for(int i = 0; i < enemy.size(); i++)
					{
						enemy[i].react();
						score += 10;
					}
				}
			}
			else if(controller_manager_.buttons_down() & SCE_CTRL_CIRCLE)
			{
				if (player.state == player.idle)
				{
					player.state = player.attack3;
					player.jump = true;

					if (player.jump == true)
					{
						for(int i = 0; i < enemy.size(); i++)
						{
							enemy[i].react();
							enemy[i].move(5, 0);
							score += 10;
						}

						game_object_.move(-5, 0);
						game_object_2.move(-5, 0);
					}
				}
			}
			else if(controller_manager_.buttons_down() & SCE_CTRL_CROSS)
			{
				if (player.state == player.idle)
				{
					player.state = player.block;
			
					for(int i = 0; i < enemy.size(); i++)
					{
						enemy[i].react();
						score += 10;
					}
				}
			}
			else if(controller_manager_.buttons_down() & SCE_CTRL_RIGHT)
			{
				player.state = player.run;
				game_object_.move(-2, 0);
				game_object_2.move(-2, 0);
			}
			else if(controller_manager_.buttons_down() & SCE_CTRL_LEFT)
			{
				player.state = player.run;
				game_object_.move(2, 0);
				game_object_2.move(2, 0);
			}
			else if(controller_manager_.buttons_released() & (SCE_CTRL_SQUARE | SCE_CTRL_TRIANGLE | SCE_CTRL_CIRCLE | SCE_CTRL_CROSS | SCE_CTRL_LEFT | SCE_CTRL_RIGHT)) 
			{
				//reset player animations
				player.texture_x = 0.0f;
				player.state = player.idle;
			}
		}
	
		player.update();

		for(int i = 0; i < enemy.size(); i++)
		{
			enemy[i].update();
		}

		background();

		bool game_running = true;
		if (controller_manager_.buttons_down() & (SCE_CTRL_START | SCE_CTRL_SELECT) == (SCE_CTRL_START | SCE_CTRL_SELECT))
			game_running = false;
		return game_running;
	}
}

void SpriteApplication::Render()
{
	// set up sprite renderer for drawing
	sprite_renderer_->Begin();

	//
	// draw all sprites here
	//
	sprite_renderer_->DrawSprite(game_object_);
	sprite_renderer_->DrawSprite(game_object_2);
	sprite_renderer_->DrawSprite(player);

	for(int i = 0; i < enemy.size(); i++)
	{
		sprite_renderer_->DrawSprite(enemy[i]);
	}

	font_.RenderText(sprite_renderer_, abfw::Vector3(10.0f, 5.0f, -0.9f), 1.0f, 0xff0000ff, abfw::TJ_LEFT, "TMNT");
	font_.RenderText(sprite_renderer_, abfw::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "FPS: %.1f", frame_rate_);
	font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width(), 5.0f, -0.9f), 1.0f, 0xffffffff, abfw::TJ_RIGHT, "Lives: %.f", player.lives);
	font_.RenderText(sprite_renderer_, abfw::Vector3(0.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, abfw::TJ_LEFT, "Score: %.f", score);

	// tell sprite renderer that all sprites have been drawn
	sprite_renderer_->End();
}

void SpriteApplication::Collide()
{
	for(int i = 0; i < enemy.size(); i++)
	{ 
		if(enemy[i].state != enemy[i].death)
		{
			if(enemy[i].position().x <= player.GetX() + 50)
			{
				enemy[i].range = true;
			}
			else enemy[i].range = false;

			if (enemy[i].GetX() > platform_->width())
			{
				enemy[i].state = enemy[i].run;
			}
		}

		if (enemy[i].attacks)
		{
			if (player.state == player.idle || player.state == player.run)
			{
				if ((enemy[i].GetX() - enemy[i].width()/2) < (player.GetX() + player.w/2))
				{
					player.state = player.hurt;
					player.apply_damage();
					player.invincible = true;
				}
			}
		}

		if (player.state == player.hurt)
		{
			enemy[i].state = enemy[i].walk;
			enemy[i].attacks = false;
		}
		else if (enemy[i].range != true)
		{
			enemy[i].state = enemy[i].run;
		}

		//bounding box collision
		if ((player.state == player.attack || player.state == player.attack2 || player.state == player.attack3) && (enemy[i].state != enemy[i].block) && (enemy[i].state != enemy[i].death))
		{
			if ((enemy[i].x) < (player.x + player.w))
			{ 
				enemy[i].state = enemy[i].death;
			}
		}

		//reset enemies if they die
		if (enemy[i].state == enemy[i].death)
		{
			if (!enemy[i].anim)
			{
				enemy[i].set_position(abfw::Vector3(player.GetX() + platform_->width(), 300.0f, 0.0f));
				enemy[i].set_width(336.0f);
				enemy[i].set_height(332.5f);
				enemy[i].set_texture(enemy_texture);
				enemy[i].init();
			}
		}
	}
}

void SpriteApplication::background()
{
	//Decide which background object is active by checking whether the player is within its bounds
	if ((player.GetX() < (game_object_.GetX() + game_object_.width()/2)) && (player.GetX() > (game_object_.GetX() - game_object_.width()/2)))
	{
		game_object_.active = true;
	}
	else
	{
		game_object_.active = false;
	}

	if (game_object_.active != true)
	{
		game_object_2.active = true;
	}
	else
	{
		game_object_2.active = false;
	}

	//Make the non-active background object translate to the left or right of the player depending on the players current position
	if (game_object_.active)
	{
		if (player.GetX() < game_object_.GetX())
		{
			game_object_2.set_position(abfw::Vector3((game_object_.GetX() - ((Int32)platform_->width()*2)), game_object_2.position().y, 0.f));
		}
		else 
		{
			game_object_2.set_position(abfw::Vector3((game_object_.GetX() + (platform_->width()*2)), game_object_2.position().y, 0.f)); 
		}
	}
	else
	{
		if (player.GetX() < game_object_2.GetX())
		{
			game_object_.set_position(abfw::Vector3((game_object_2.GetX() - ((Int32)platform_->width()*2)), game_object_.position().y, 0.f));
		}
		else 
		{
			game_object_.set_position(abfw::Vector3((game_object_2.GetX() + (platform_->width()*2)), game_object_.position().y, 0.f));
		}
	}
}