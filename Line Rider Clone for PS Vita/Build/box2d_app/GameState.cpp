#include "GameState.h"

GameState::GameState(abfw::Platform* platform) :
	world_(NULL)
{
	platform_ = platform;
}

GameState::~GameState()
{
}

void GameState::Enter(abfw::AudioManagerVita* audio_manager_)
{
	//initialise Box2D World Gravity
	b2Vec2 gravity(0.0f, -10.0f);
	world_ = new b2World(gravity);

	//initialise other variables
	for(int i = 0; i < player_body.size(); i++)
	{
		player_body[i] = NULL;
	}

	Left_wheel_joint = NULL;
	Right_wheel_joint = NULL;
	Eraser = NULL;
	touch_updated = false;
	player_placed = false;
	newly_entered_draw_curve = false;
	engine_running = false;
	friction_timer = 120; //since frame_tick has not been passed, this will count down once per frame to not allow another voice to activate until first is finished playing (sound is 2 sec long)
	friction_bool = false;

	//Load music
	engine_sound = audio_manager_->LoadSample("Race Car Idle-SoundBible.com-804330831.wav", *platform_);
	game_music = audio_manager_->LoadMusic("bensound-psychedelic.wav", *platform_);
	friction_sound = audio_manager_->LoadSample("Rustle-SoundBible.com-1736422480.wav", *platform_);

	volume_info.volume = 1.0f;
	volume_info.pan = 0.0f;

	if(game_music != -1)
	{ 
		//play background music if file loaded correctly
		audio_manager_->PlayMusic();
	}
	
	mode = draw_enter;

	Draw();
}

State* GameState::Update(abfw::TouchInputManager* touch_manager_, abfw::ControllerInputManagerVita& controller_manager_, abfw::AudioManagerVita* audio_manager_, float ticks)
{
	controller_manager_.Update();

	float timeStep = 1.0f/60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	if(world_)
	{
		world_->Step(
				timeStep,
				velocityIterations,
				positionIterations);
	}

	Touch_input(touch_manager_);

	Collision_response(audio_manager_);

	if(mode == place_character)
	{
		if(player == Circle_player)
		{
			if(character_selection == 0)
			{
				//Animate player character
				float texture_pos = player_vita.Animate(4, 10, (1.0f/4.0f), ticks);
				player_vita.set_uv_position(abfw::Vector2(texture_pos, 0));
			}
			else
			{
				float texture_pos = player_vita.Animate(6, 10, (1.0f/6.0f), ticks);
				player_vita.set_uv_position(abfw::Vector2(texture_pos, 0));
			}
		}
		else if(player == Car_player)
		{
			Controller_input(controller_manager_);
			
			if(character_selection == 0)
			{
				//Animate player character
				float texture_pos = left_wheel_sprite.Animate(4, 10, (1.0f/4.0f), ticks);
				left_wheel_sprite.set_uv_position(abfw::Vector2(texture_pos, 0));
				right_wheel_sprite.set_uv_position(abfw::Vector2(texture_pos, 0));
			}
			else
			{
				float texture_pos = left_wheel_sprite.Animate(6, 10, (1.0f/6.0f), ticks);
				left_wheel_sprite.set_uv_position(abfw::Vector2(texture_pos, 0));
				right_wheel_sprite.set_uv_position(abfw::Vector2(texture_pos, 0));
			}

			car_body_sprite.set_rotation(-player_body[0]->GetAngle());

			//play engine sound for car if it has loaded correctly, only play it once
			if(engine_sound != -1)
			{
				if(!engine_running)
				{
					audio_manager_->PlaySample(engine_sound, true);
					engine_running = true;
				}
			}
		}
	}

	return this;
}

void GameState::Collision_response(abfw::AudioManagerVita* audio_manager_)
{
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	for(int contact_num = 0; contact_num < contact_count; ++contact_num)
	{
		if(contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			//Sound effect collision response
			if(mode == place_character)
			{
				for (std::vector<b2Body*>::iterator edge_iter = draw_lines.edge.begin(); edge_iter != draw_lines.edge.end(); )
				{
					if(bodyA != NULL && (bodyB == player_body[0] || bodyB == player_body[1]))
					{
						if((*edge_iter) == bodyA)
						{
							volume_info.volume = 0.0f;
							game_music = audio_manager_->SetSampleVoiceVolumeInfo(game_music, volume_info);

							if(friction_sound != -1)
							{
								if(!friction_sound_timer()) //check if another sample is already playing
								{
									audio_manager_->PlaySample(friction_sound, false);
									friction_bool = true;
									friction_sound_timer();

									if(friction_sound_timer())
									{
										friction_bool = false;
									}
								}
							}

							break;
						}
						else
						{
							edge_iter++;
						}
					}
					else if((bodyA == player_body[0] || bodyA == player_body[1]) && bodyB != NULL)
					{
						if((*edge_iter) == bodyB)
						{
							volume_info.volume = 0.0f;
							game_music = audio_manager_->SetSampleVoiceVolumeInfo(game_music, volume_info);

							if(friction_sound != -1)
							{
								if(!friction_sound_timer()) //check if another sample is already playing
								{
									audio_manager_->PlaySample(friction_sound, false);
									friction_bool = true;
									friction_sound_timer();

									if(friction_sound_timer())
									{
										friction_bool = false;
									}
								}
							}

							break;
						}
						else
						{
							edge_iter++;
						}
					}
					else
					{
						edge_iter++;
					}
				}

				for (std::vector<b2Body*>::iterator edge_iter = draw_curves.edge.begin(); edge_iter != draw_curves.edge.end(); )
				{
					if(bodyA != NULL && (bodyB == player_body[0] || bodyB == player_body[1]))
					{
						if((*edge_iter) == bodyA)
						{
							volume_info.volume = 0.0f;
							game_music = audio_manager_->SetSampleVoiceVolumeInfo(game_music, volume_info);
							
							if(friction_sound != -1)
							{
								if(!friction_sound_timer()) //check if another sample is already playing
								{
									audio_manager_->PlaySample(friction_sound, false);
									friction_bool = true;
									friction_sound_timer();

									if(friction_sound_timer())
									{
										friction_bool = false;
									}
								}
							}

							break;
						}
						else
						{
							edge_iter++;
						}
					}
					else if((bodyA == player_body[0] || bodyA == player_body[1]) && bodyB != NULL)
					{
						if((*edge_iter) == bodyB)
						{
							volume_info.volume = 0.0f;
							game_music = audio_manager_->SetSampleVoiceVolumeInfo(game_music, volume_info);
							
							if(friction_sound != -1)
							{
								if(!friction_sound_timer()) //check if another sample is already playing
								{
									audio_manager_->PlaySample(friction_sound, false);
									friction_bool = true;
									friction_sound_timer();

									if(friction_sound_timer())
									{
										friction_bool = false;
									}
								}
							}

							break;
						}
						else
						{
							edge_iter++;
						}
					}
					else
					{
						edge_iter++;
					}
				}
			}
			else
			{
				volume_info.volume = 1.0f;
				game_music = audio_manager_->SetSampleVoiceVolumeInfo(game_music, volume_info);
			}

			//Eraser collision response
			if(mode == draw_erase)
			{
				if(bodyA != NULL && bodyB == Eraser)
				{	
					if((bodyA != player_body[0] || bodyA != player_body[1]))
					{
						//use GetUserData to delete sprite from draw_lines.line and draw_curves.line

						Sprite* spriteA = (Sprite*)bodyA->GetUserData();
						
						//trying to stop the eraser from trying to delete the buttons
						if(spriteA != &eraser_button || spriteA != &line_button || spriteA != &curve_button || spriteA != &clear_button || spriteA != &place_player_button || spriteA != &game_back_button || spriteA != &player_eraser)
						{
							std::vector<abfw::Sprite>::iterator draw_lines_line_iter = draw_lines.line.begin();
							for (std::vector<b2Body*>::iterator edge_iter = draw_lines.edge.begin(); edge_iter != draw_lines.edge.end(); )
							{
								if((*edge_iter) == bodyA)
								{
									edge_iter = draw_lines.edge.erase(edge_iter);
									draw_lines_line_iter = draw_lines.line.erase(draw_lines_line_iter);
									delete_physics(bodyA);
									break;
								}
								else
								{
									edge_iter++;
									draw_lines_line_iter++;
								}
							}


							std::vector<abfw::Sprite>::iterator draw_curves_line_iter = draw_curves.line.begin();
							for (std::vector<b2Body*>::iterator edge_iter = draw_curves.edge.begin(); edge_iter != draw_curves.edge.end(); )
							{
								if((*edge_iter) == bodyA)
								{
									edge_iter = draw_curves.edge.erase(edge_iter);
									draw_curves_line_iter = draw_curves.line.erase(draw_curves_line_iter);
									delete_physics(bodyA);
									break;
								}
								else
								{
									edge_iter++;
									draw_curves_line_iter++;
								}
							}
						}

						delete spriteA;
						spriteA = NULL;
					}
				}

				if(bodyA == Eraser && bodyB != NULL)
				{	
					if((bodyB != player_body[0] || bodyB != player_body[1]))
					{
						//use GetUserData to delete sprite from draw_lines.line and draw_curves.line

						Sprite* spriteB = (Sprite*)bodyB->GetUserData();
						
						//trying to stop the eraser from trying to delete the buttons
						if(spriteB != &eraser_button || spriteB != &line_button || spriteB != &curve_button || spriteB != &clear_button || spriteB != &place_player_button || spriteB != &game_back_button || spriteB != &player_eraser)
						{
							std::vector<abfw::Sprite>::iterator draw_lines_line_iter = draw_lines.line.begin();
							for (std::vector<b2Body*>::iterator edge_iter = draw_lines.edge.begin(); edge_iter != draw_lines.edge.end(); )
							{
								if((*edge_iter) == bodyB)
								{
									edge_iter = draw_lines.edge.erase(edge_iter);
									draw_lines_line_iter = draw_lines.line.erase(draw_lines_line_iter);
									delete_physics(bodyB);
									break;
								}
								else
								{
									edge_iter++;
									draw_lines_line_iter++;
								}
							}


							std::vector<abfw::Sprite>::iterator draw_curves_line_iter = draw_curves.line.begin();
							for (std::vector<b2Body*>::iterator edge_iter = draw_curves.edge.begin(); edge_iter != draw_curves.edge.end(); )
							{
								if((*edge_iter) == bodyB)
								{
									edge_iter = draw_curves.edge.erase(edge_iter);
									draw_curves_line_iter = draw_curves.line.erase(draw_curves_line_iter);
									delete_physics(bodyB);
									break;
								}
								else
								{
									edge_iter++;
									draw_curves_line_iter++;
								}
							}
						}

						delete spriteB;
						spriteB = NULL;
					}
				}
			}

			// Get next contact point
			contact = contact->GetNext();
			break;
		}
	}
}

void GameState::Controller_input(abfw::ControllerInputManagerVita controller_manager_)
{
	if(controller_manager_.buttons_pressed() & SCE_CTRL_CROSS)
	{
		if(!Left_wheel_joint->IsMotorEnabled())
		{
			Left_wheel_joint->EnableMotor(true);
			Right_wheel_joint->EnableMotor(true);
			Left_wheel_joint->SetMaxMotorTorque(50.0f);
			Right_wheel_joint->SetMaxMotorTorque(50.0f);
		}

		Left_wheel_joint->SetMotorSpeed(-10.0f);
		Right_wheel_joint->SetMotorSpeed(-10.0f);
	}
	else if(controller_manager_.buttons_down() & SCE_CTRL_SQUARE)
	{
		if(!Left_wheel_joint->IsMotorEnabled())
		{
			Left_wheel_joint->EnableMotor(true);
			Right_wheel_joint->EnableMotor(true);
			Left_wheel_joint->SetMaxMotorTorque(50.0f);
			Right_wheel_joint->SetMaxMotorTorque(50.0f);
		}

		Left_wheel_joint->SetMotorSpeed(10.0f);
		Right_wheel_joint->SetMotorSpeed(10.0f);
	}
}

void GameState::Touch_input(abfw::TouchInputManager* touch_manager_)
{
	const abfw::Touch& first_touch = touch_manager_->GetTouch(0, 0);

	//change drawing modes
	if(first_touch.type != abfw::TT_NONE)
	{
		if((first_touch.position.x > line_button.GetX()) && (first_touch.position.x < (line_button.GetX() + line_button.width())))
		{
			if((first_touch.position.y > line_button.GetY()) && (first_touch.position.y < (line_button.GetY() + line_button.height())))
			{
				mode = draw_line;
			}
		}

		if((first_touch.position.x > curve_button.GetX()) && (first_touch.position.x < (curve_button.GetX() + curve_button.width())))
		{
			if((first_touch.position.y > curve_button.GetY()) && (first_touch.position.y < (curve_button.GetY() + curve_button.height())))
			{
				mode = draw_curve;
				newly_entered_draw_curve = true;
			}
		}

		if((first_touch.position.x > eraser_button.GetX()) && (first_touch.position.x < (eraser_button.GetX() + eraser_button.width())))
		{
			if((first_touch.position.y > eraser_button.GetY()) && (first_touch.position.y < (eraser_button.GetY() + eraser_button.height())))
			{
				mode = draw_erase;
			}
		}

		if((first_touch.position.x > place_player_button.GetX()) && (first_touch.position.x < (place_player_button.GetX() + place_player_button.width())))
		{
			if((first_touch.position.y > place_player_button.GetY()) && (first_touch.position.y < (place_player_button.GetY() + place_player_button.height())))
			{
				mode = place_character;
			}
		}

		if((first_touch.position.x > clear_button.GetX()) && (first_touch.position.x < (clear_button.GetX() + clear_button.width())))
		{
			if((first_touch.position.y > clear_button.GetY()) && (first_touch.position.y < (clear_button.GetY() + clear_button.height())))
			{
				draw_lines.clear_world();
				draw_curves.clear_world();
			}
		}
	}

	if (mode == draw_line)
	{
		//get first place the player touches
		if(first_touch.type == abfw::TT_NEW)
		{
			draw_lines.v1 = b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y));
		}

		//get the last place the player touches
		if(first_touch.type == abfw::TT_RELEASED)
		{
			draw_lines.v2 = b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y));

			touch_updated = true;
		}

		//draw the line
		if (touch_updated)
		{
			draw_lines.line.push_back(draw_lines.sprite_edge);
			draw_lines.edge.push_back(draw_lines.b2_edge);

			for (int f = draw_lines.edge.size() - 1; f < draw_lines.edge.size(); f++)
			{
				draw_lines.draw_lines(draw_lines.v1, draw_lines.v2, f, world_);
			}
		
			touch_updated = false;
		}

		line_button.set_uv_position(abfw::Vector2(0.5f, 0));
	}
	else
	{
		line_button.set_uv_position(abfw::Vector2(0, 0));
	}

	if (mode == draw_curve)
	{
		//stops the first point of the curve being registered under the button when the state changes
		if (newly_entered_draw_curve)
		{	
			draw_curves.point.clear();
			newly_entered_draw_curve = false;
		}

		//get the players desired 5 points
		if(first_touch.type == abfw::TT_NEW)
		{
			draw_curves.point.push_back(b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y)));		

			if (draw_curves.point.size() > 4)
			{
				touch_updated = true;
			}
		}

		//draw the curve
		if (touch_updated)
		{
			draw_curves.calculate_bezier_curve(draw_curves.point, world_);

			draw_curves.point.clear();
			touch_updated = false;
		}

		curve_button.set_uv_position(abfw::Vector2(0.5f, (1.0f/6.0f)));
	}	
	else
	{
		curve_button.set_uv_position(abfw::Vector2(0, (1.0f/6.0f)));
	}

	if (mode == place_character)
	{
		if(first_touch.type != abfw::TT_NONE)
		{
			delete_physics(Eraser);

			if(player_selection == 0)
			{
				//stop more than player physics body to be on screen at once
				for(int i = 0; i < player_body.size(); i++)
				{
					delete_physics(player_body[i]);
				}

				player_body.clear();

				player = Circle_player;
				create_player(first_touch);
			}

			if(player_selection == 1)
			{
				for(int i = 0; i < player_body.size(); i++)
				{
					delete_physics(player_body[i]);
					Left_wheel_joint = NULL;
					Right_wheel_joint = NULL;
				}

				player_body.clear();

				player = Car_player;
				create_player(first_touch);
			}
		}

		if(player == Circle_player)
		{
			player_vita.set_position(abfw::Vector3(BOX2D_GFX_POS_X(player_body[0]->GetPosition().x), BOX2D_GFX_POS_Y(player_body[0]->GetPosition().y), 0.0f));
			player_body[0]->SetUserData(&player_vita);
		}

		if(player == Car_player)
		{
			car_body_sprite.set_position(abfw::Vector3(BOX2D_GFX_POS_X(player_body[0]->GetPosition().x), BOX2D_GFX_POS_Y(player_body[0]->GetPosition().y), 0.0f));
			player_body[0]->SetUserData(&car_body_sprite);

			left_wheel_sprite.set_position(abfw::Vector3(BOX2D_GFX_POS_X(player_body[1]->GetPosition().x), BOX2D_GFX_POS_Y(player_body[1]->GetPosition().y), 0.0f));
			player_body[1]->SetUserData(&left_wheel_sprite);

			right_wheel_sprite.set_position(abfw::Vector3(BOX2D_GFX_POS_X(player_body[2]->GetPosition().x), BOX2D_GFX_POS_Y(player_body[2]->GetPosition().y), 0.0f));
			player_body[2]->SetUserData(&right_wheel_sprite);
		}

		place_player_button.set_uv_position(abfw::Vector2(0.5f, (2.0f/3.0f)));
	}
	else
	{
		place_player_button.set_uv_position(abfw::Vector2(0, (2.0f/3.0f)));
	}

	if (mode == draw_erase)
	{
		if(first_touch.type == abfw::TT_NEW)
		{
			//Allow the physics body to move with the players finger
			Eraser = CircleBody((b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y))), 0.125, 1, 1, 0, world_);
			Mouse = MouseJoint((b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y))), Ground, Eraser, world_);
		}
		else if(first_touch.type == abfw::TT_ACTIVE)
		{
			Mouse->SetTarget(b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y)));
			player_eraser.set_position(abfw::Vector3(BOX2D_GFX_POS_X(Eraser->GetPosition().x), BOX2D_GFX_POS_Y(Eraser->GetPosition().y), 0));
		}
		else if(first_touch.type == abfw::TT_RELEASED)
		{
			delete_physics(Eraser);
		}

		eraser_button.set_uv_position(abfw::Vector2(0.5f, (1.0f/3.0f)));
	}
	else
	{
		eraser_button.set_uv_position(abfw::Vector2(0, (1.0f/3.0f)));
	}
}

void GameState::Draw()
{
	Ground = StaticBody_sensorless(b2Vec2(0, -1), platform_->width(), 0.05, world_); 

	float section = (platform_->width())/6; 
	float button_width = 50;
	float button_height = 50;

	//each button will sit at the bottom of the screen - 6 buttons will split will screen into 6 sections (960/6 = 160) and the button will sit in the centre of each section
	line_button.set_height(button_height);
	line_button.set_width(button_width);
	line_button.set_position(abfw::Vector3((section*0.5), 490, 0));
	line_button.set_uv_height(1.0f/6.0f);
	line_button.set_uv_width(1.0f/2.0f);
	line_button.set_uv_position(abfw::Vector2(0, 0));

	curve_button.set_height(button_height);
	curve_button.set_width(button_width);
	curve_button.set_position(abfw::Vector3((section*1.5), 490, 0));
	curve_button.set_uv_height(1.0f/6.0f);
	curve_button.set_uv_width(1.0f/2.0f);
	curve_button.set_uv_position(abfw::Vector2(0, (1.0f/6.0f)));

	eraser_button.set_height(button_height);
	eraser_button.set_width(button_width);
	eraser_button.set_position(abfw::Vector3((section*2.5), 490, 0));
	eraser_button.set_uv_height(1.0f/6.0f);
	eraser_button.set_uv_width(1.0f/2.0f);
	eraser_button.set_uv_position(abfw::Vector2(0, (1.0f/3.0f)));

	clear_button.set_height(button_height);
	clear_button.set_width(button_width);
	clear_button.set_position(abfw::Vector3((section*3.5), 490, 0));
	clear_button.set_uv_height(1.0f/6.0f);
	clear_button.set_uv_width(1.0f/2.0f);
	clear_button.set_uv_position(abfw::Vector2(0, (1.0f/2.0f)));

	place_player_button.set_height(button_height);
	place_player_button.set_width(button_width);
	place_player_button.set_position(abfw::Vector3((section*4.5), 490, 0));
	place_player_button.set_uv_height(1.0f/6.0f);
	place_player_button.set_uv_width(1.0f/2.0f);
	place_player_button.set_uv_position(abfw::Vector2(0, (2.0f/3.0f)));

	game_back_button.set_height(button_height);
	game_back_button.set_width(button_width);
	game_back_button.set_position(abfw::Vector3((section*5.5), 490, 0));
	game_back_button.set_uv_height(1.0f/6.0f);
	game_back_button.set_uv_width(1.0f/2.0f);
	game_back_button.set_uv_position(abfw::Vector2(0, (5.0f/6.0f)));

	if(character_selection == 0)
	{
		player_vita.set_height(button_height*2);
		player_vita.set_width(button_width*2);
		player_vita.set_uv_width(1.0f/4.0f);

		left_wheel_sprite.set_height(button_height*2);
		left_wheel_sprite.set_width(button_width*2);
		left_wheel_sprite.set_uv_width(1.0f/4.0f);

		right_wheel_sprite.set_height(button_height*2);
		right_wheel_sprite.set_width(button_width*2);
		right_wheel_sprite.set_uv_width(1.0f/4.0f);
	}
	else
	{
		player_vita.set_height(button_height*2);
		player_vita.set_width(button_width*2);
		player_vita.set_uv_width(1.0f/6.0f);

		left_wheel_sprite.set_height(button_height*2);
		left_wheel_sprite.set_width(button_width*2);
		left_wheel_sprite.set_uv_width(1.0f/6.0f);

		right_wheel_sprite.set_height(button_height*2);
		right_wheel_sprite.set_width(button_width*2);
		right_wheel_sprite.set_uv_width(1.0f/6.0f);
	}

	car_body_sprite.set_height(button_height*2);
	car_body_sprite.set_width(button_width*2);

	player_eraser.set_height(button_height/2);
	player_eraser.set_width(button_width/2);
	player_eraser.set_colour(0xff493Bf0);

	background.set_height(platform_->height());
	background.set_width(platform_->width());
	background.set_position(abfw::Vector3(platform_->width()/2, platform_->height()/2, 0));
}

Draw_LINES GameState::get_draw_lines()
{
	return draw_lines;
}

Draw_LINES GameState::get_draw_curves()
{
	return draw_curves;
}

bool GameState::get_player_placed()
{
	return player_placed;
}

void GameState::create_player(const abfw::Touch& first_touch)
{
	if(player == Circle_player)
	{
		player_body.push_back(CircleBody((b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y))), 0.5f, 1.0f, 0.5f, 0.0f, world_)); //hamsterball
	}

	if(player == Car_player)
	{
		player_body.push_back(DynamicBody(b2Vec2(GFX_BOX2D_POS_X(first_touch.position.x), GFX_BOX2D_POS_Y(first_touch.position.y)), 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, world_)); //car body
		player_body.push_back(CircleBody(b2Vec2((player_body[0]->GetPosition().x - 0.5f), (player_body[0]->GetPosition().y - 0.5f)), 0.5f, 1.0f, 0.5f, 0.0f, world_)); //left wheel
		player_body.push_back(CircleBody(b2Vec2((player_body[0]->GetPosition().x + 0.5f), (player_body[0]->GetPosition().y - 0.5f)), 0.5f, 1.0f, 0.5f, 0.0f, world_)); //right wheel
		Left_wheel_joint = RevoluteJoint(player_body[0], player_body[1], b2Vec2((player_body[0]->GetPosition().x - 0.5f), (player_body[0]->GetPosition().y - 0.5f)), b2Vec2(player_body[1]->GetWorldCenter().x, player_body[1]->GetWorldCenter().y), 0.0f, world_); //left revolute joint
		Right_wheel_joint = RevoluteJoint(player_body[0], player_body[2], b2Vec2((player_body[0]->GetPosition().x + 0.5f), (player_body[0]->GetPosition().y - 0.5f)), b2Vec2(player_body[2]->GetWorldCenter().x, player_body[2]->GetWorldCenter().y), 0.0f, world_); //right revolute joint
	}
}

bool GameState::friction_sound_timer()
{
	if(friction_bool)
	{
		friction_timer--;

		if(friction_timer <= 0)
		{
			friction_timer = 120;
			return false;			
		}
		else if(friction_timer == 120)
		{
			return false;
		}
		else if(friction_timer > 0 && friction_timer < 120)
		{
			return true;
		}
	}
}

void GameState::delete_physics(b2Body* &object)
{
	if(object != NULL)
	{
		b2Body* thisObject = object;
		b2World* thisWorld = thisObject->GetWorld();
		thisWorld->DestroyBody(object);
		object = NULL;
	}
}

void GameState::Exit(abfw::AudioManagerVita* audio_manager_)
{
	draw_curves.clear_world();
	draw_lines.clear_world();

	audio_manager_->StopMusic(); //stop music when leaving the game

	audio_manager_->UnloadSample(friction_sound);
	audio_manager_->UnloadSample(game_music);
	audio_manager_->UnloadSample(engine_sound);

 	delete world_;
	world_ = NULL;
}
