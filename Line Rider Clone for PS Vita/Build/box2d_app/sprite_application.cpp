#include "sprite_application.h"

SpriteApplication::SpriteApplication(abfw::Platform* platform) :
	abfw::Application(platform),
	sprite_renderer_(NULL),
	theGameState(platform),
	theMenuState(platform),
	theOptionsState(platform),
	theHowtoplayState(platform),
	player_texture(NULL),
	button_texture(NULL),
	game_button_texture(NULL),
	game_background_texture(NULL),
	title_texture(NULL),
	options_background_texture(NULL),
	howtoplay_background_texture(NULL),
	car_texture(NULL),
	player_texture2(NULL),
	options_button_texture(NULL),
	next_state(NULL), 
	touch_manager_(NULL),
	audio_manager_(NULL)
{
}

SpriteApplication::~SpriteApplication()
{
}

void SpriteApplication::Init()
{
	//initialise touch input
	touch_manager_ = platform_->CreateTouchInputManager();
	touch_manager_->EnablePanel(0);

	//create an audio manager
	audio_manager_ = new abfw::AudioManagerVita;

	// load the font to draw the on-screen text
	bool font_loaded = font_.Load("comic_sans", *platform_);
	if(!font_loaded)
	{
		std::cout << "Font failed to load." << std::endl;
		exit(-1);
	}

	// create a sprite renderer to draw the sprites
	sprite_renderer_ = platform_->CreateSpriteRenderer();

	//load textures
	player_texture = load_texture("mouse_in_a_ball.png");
	button_texture = load_texture("buttons.png");
	title_texture = load_texture("title2.png");
	game_button_texture = load_texture("game_buttons.png");
	game_background_texture = load_texture("game_background.png");
	options_background_texture = load_texture("options_background.png");
	howtoplay_background_texture = load_texture("howtoplay_background.png");
	car_texture = load_texture("car_body.png");
	player_texture2 = load_texture("penguin_ball.png");
	options_button_texture = load_texture("options_buttons.png");

	//set textures to corresponding bodies
	theMenuState.title_screen.set_texture(title_texture);
	theMenuState.start_game.set_texture(button_texture);
	theMenuState.options.set_texture(button_texture);
	theMenuState.credits.set_texture(button_texture);

	theOptionsState.options_screen.set_texture(options_background_texture);
	theOptionsState.start_game_button.set_texture(button_texture);
	theOptionsState.choose_ball.set_texture(options_button_texture);
	theOptionsState.choose_car.set_texture(options_button_texture);
	theOptionsState.choose_mouse.set_texture(options_button_texture);
	theOptionsState.choose_penguin.set_texture(options_button_texture);

	theHowtoplayState.howtoplay_background.set_texture(howtoplay_background_texture);
	theHowtoplayState.main_menu_button.set_texture(button_texture);

	theGameState.line_button.set_texture(game_button_texture);
	theGameState.curve_button.set_texture(game_button_texture);
	theGameState.eraser_button.set_texture(game_button_texture);
	theGameState.clear_button.set_texture(game_button_texture);
	theGameState.place_player_button.set_texture(game_button_texture);
	theGameState.game_back_button.set_texture(game_button_texture);
	theGameState.background.set_texture(game_background_texture);
	theGameState.car_body_sprite.set_texture(car_texture);

	//initialise states
	AppCurrentState = NULL;
	next_state = &theMenuState;
}

void SpriteApplication::CleanUp()
{
	// free up the sprite renderer
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete player_texture;
	player_texture = NULL;

	delete title_texture;
	title_texture = NULL;

	delete button_texture;
	button_texture = NULL;
	
	delete touch_manager_;
	touch_manager_ = NULL;

	delete audio_manager_;
	audio_manager_ = NULL;

	delete next_state;
	next_state = NULL;
}

bool SpriteApplication::Update(float ticks)
{
	// calculate the frame rate
	frame_rate_ = 1.0f / ticks;

	Touch_input();

	//switch state if pointer is different, else update current state
	if(next_state != AppCurrentState)
	{
		if(AppCurrentState)
			AppCurrentState->Exit(audio_manager_);
		next_state->Enter(audio_manager_);
		AppCurrentState = next_state;
	}
	else
	{
		next_state = AppCurrentState->Update(touch_manager_, controller_manager_, audio_manager_, ticks);
	}

	return true;
}

void SpriteApplication::Render()
{
	// set up sprite renderer for drawing
	sprite_renderer_->Begin();

	//
	// draw all sprites here
	//

	if(AppCurrentState == &theMenuState)
	{
		sprite_renderer_->DrawSprite(theMenuState.title_screen);
		sprite_renderer_->DrawSprite(theMenuState.start_game);
		sprite_renderer_->DrawSprite(theMenuState.options);
		sprite_renderer_->DrawSprite(theMenuState.credits);
	}

	if(AppCurrentState == &theOptionsState)
	{
		sprite_renderer_->DrawSprite(theOptionsState.options_screen);
		sprite_renderer_->DrawSprite(theOptionsState.choose_ball);
		sprite_renderer_->DrawSprite(theOptionsState.choose_car);
		sprite_renderer_->DrawSprite(theOptionsState.choose_mouse);
		sprite_renderer_->DrawSprite(theOptionsState.choose_penguin);
		sprite_renderer_->DrawSprite(theOptionsState.start_game_button);
	}

	if(AppCurrentState == &theHowtoplayState)
	{
		sprite_renderer_->DrawSprite(theHowtoplayState.howtoplay_background);
		sprite_renderer_->DrawSprite(theHowtoplayState.main_menu_button);

		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "Using the line and curve tools, draw a level for your character.");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 25, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "To draw a straight line using the line tool,");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 45, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "drag your finger across the screen where you would like the line.");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 85, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "To draw a curved line using the curve tool,");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 105, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "touch the screen in 5 points in successive order and a Bezier curve will be drawn.");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 145, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "Placing the character on screen using the relevant button will start the simulation,");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 165, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "remember to place it on the drawn lines!");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 205, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "If the car is selected, use the 'X' for forward acceleration and 'Square' for reverse.");
		font_.RenderText(sprite_renderer_, abfw::Vector3(platform_->width()/2, (platform_->height())/4 + 225, -0.9f), 1.0f, 0xff000000, abfw::TJ_CENTRE, "These are the basic controls, have fun!.");
	}

	if(AppCurrentState == &theGameState)
	{
		if(theGameState.character_selection == 0)
		{
			theGameState.player_vita.set_texture(player_texture2);
			theGameState.left_wheel_sprite.set_texture(player_texture2);
			theGameState.right_wheel_sprite.set_texture(player_texture2);
		}
		else
		{
			theGameState.player_vita.set_texture(player_texture);
			theGameState.left_wheel_sprite.set_texture(player_texture);
			theGameState.right_wheel_sprite.set_texture(player_texture);
		}

		sprite_renderer_->DrawSprite(theGameState.background);

		for (int f = 0; f < theGameState.get_draw_lines().line.size(); f++)
		{
			sprite_renderer_->DrawSprite(theGameState.get_draw_lines().line[f]);
		}

		static int override_line_count = 0;

		int line_count = theGameState.get_draw_curves().line.size();

		if(override_line_count > 0)
		{
			line_count = override_line_count;
		}

		for (int f = 0; f < line_count; f++)
		{
			sprite_renderer_->DrawSprite(theGameState.get_draw_curves().line[f]);
		}

		sprite_renderer_->DrawSprite(theGameState.line_button);
		sprite_renderer_->DrawSprite(theGameState.curve_button);
		sprite_renderer_->DrawSprite(theGameState.eraser_button);
		sprite_renderer_->DrawSprite(theGameState.clear_button);
		sprite_renderer_->DrawSprite(theGameState.place_player_button);
		sprite_renderer_->DrawSprite(theGameState.game_back_button);
		
		if(theGameState.get_draw_mode() == 4)
		{
			if(theGameState.get_player() == 0)
			{
				sprite_renderer_->DrawSprite(theGameState.player_vita);
			}
			else
			{
				sprite_renderer_->DrawSprite(theGameState.car_body_sprite); 
				sprite_renderer_->DrawSprite(theGameState.left_wheel_sprite); 
				sprite_renderer_->DrawSprite(theGameState.right_wheel_sprite); 
			}
		}

		if(theGameState.get_draw_mode() == 3)
		{
			sprite_renderer_->DrawSprite(theGameState.player_eraser);
		}
		
		//Give names to draw_mode enumerated type integers for text output
		const char* mode_names[] = 
		{
			"enter",
			"line",
			"curve",
			"erase",
			"place character"
		};

		font_.RenderText(sprite_renderer_, abfw::Vector3(0, 510.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "Drawing mode: %s", mode_names[theGameState.get_draw_mode()]);
	}

	font_.RenderText(sprite_renderer_, abfw::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xff00ffff, abfw::TJ_LEFT, "FPS: %.1f", frame_rate_);

	// tell sprite renderer that all sprites have been drawn
	sprite_renderer_->End();
}

abfw::Texture* SpriteApplication::load_texture(const char *texture)
{
	png_loader.Load(texture, *platform_, image_data);

	if(image_data.image() == NULL)
	{
		std::cout << "Image load failed" << std::endl;
	}

	return platform_->CreateTexture(image_data);
}

void SpriteApplication::Touch_input()
{
	touch_manager_->Update();

	const abfw::Touch& first_touch = touch_manager_->GetTouch(0, 0);

	//change states from menustate and play bing sound on button press
	if (AppCurrentState == &theMenuState)
	{
		if(first_touch.type == abfw::TT_NEW)
		{
			if((first_touch.position.x > (theMenuState.start_game.GetX() - theMenuState.start_game.width()/2)) && (first_touch.position.x < (theMenuState.start_game.GetX() + theMenuState.start_game.width()/2)))
			{
				if((first_touch.position.y > (theMenuState.start_game.GetY() - theMenuState.start_game.height()/2)) && (first_touch.position.y < (theMenuState.start_game.GetY() + theMenuState.start_game.height()/2)))
				{
					theMenuState.start_game.set_uv_position(abfw::Vector2(0.5f, 0.0f));

					if(theMenuState.bing_sound != -1)
					{
						audio_manager_->PlaySample(theMenuState.bing_sound, false);
					}
				}
			}

			if((first_touch.position.x > (theMenuState.options.GetX() - theMenuState.options.width()/2)) && (first_touch.position.x < (theMenuState.options.GetX() + theMenuState.options.width()/2)))
			{
				if((first_touch.position.y > (theMenuState.options.GetY() - theMenuState.options.height()/2)) && (first_touch.position.y < (theMenuState.options.GetY() + theMenuState.options.height()/2)))
				{
					theMenuState.options.set_uv_position(abfw::Vector2((1.0f/2.0f), (1.0f/4.0f)));

					if(theMenuState.bing_sound != -1)
					{
						audio_manager_->PlaySample(theMenuState.bing_sound, false);
					}
				}
			}

			if((first_touch.position.x > (theMenuState.credits.GetX() - theMenuState.credits.width()/2)) && (first_touch.position.x < (theMenuState.credits.GetX() + theMenuState.credits.width()/2)))
			{
				if((first_touch.position.y > (theMenuState.credits.GetY() - theMenuState.credits.height()/2)) && (first_touch.position.y < (theMenuState.credits.GetY() + theMenuState.credits.height()/2)))
				{
					theMenuState.credits.set_uv_position(abfw::Vector2((1.0/2.0), (1.0f/2.0f)));

					if(theMenuState.bing_sound != -1)
					{
						audio_manager_->PlaySample(theMenuState.bing_sound, false);
					}
				}
			}
		}
		else if(first_touch.type == abfw::TT_RELEASED)
		{
			if((first_touch.position.x > (theMenuState.start_game.GetX() - theMenuState.start_game.width()/2)) && (first_touch.position.x < (theMenuState.start_game.GetX() + theMenuState.start_game.width()/2)))
			{
				if((first_touch.position.y > theMenuState.start_game.GetY() - theMenuState.start_game.height()/2) && (first_touch.position.y < (theMenuState.start_game.GetY() + theMenuState.start_game.height()/2)))
				{
					next_state = &theGameState;
					theGameState.player_selection = 0; //default value, hamsterball
					theGameState.character_selection = 0; //default value, penguin
				}
			}

			if((first_touch.position.x > (theMenuState.options.GetX() - theMenuState.options.width()/2)) && (first_touch.position.x < (theMenuState.options.GetX() + theMenuState.options.width()/2)))
			{
				if((first_touch.position.y > (theMenuState.options.GetY() - theMenuState.options.height()/2)) && (first_touch.position.y < (theMenuState.options.GetY() + theMenuState.options.height()/2)))
				{
					next_state = &theOptionsState;
				}
			}

			if((first_touch.position.x > (theMenuState.credits.GetX() - theMenuState.credits.width()/2)) && (first_touch.position.x < (theMenuState.credits.GetX() + theMenuState.credits.width()/2)))
			{
				if((first_touch.position.y > (theMenuState.credits.GetY() - theMenuState.credits.height()/2)) && (first_touch.position.y < (theMenuState.credits.GetY() + theMenuState.credits.height()/2)))
				{
					next_state = &theHowtoplayState;
				}
			}
		}
	}

	//return to main menu from game
	if (AppCurrentState == &theGameState)
	{
		if((first_touch.position.x > (theGameState.game_back_button.GetX() - theGameState.game_back_button.width()/2)) && (first_touch.position.x < (theGameState.game_back_button.GetX() + theGameState.game_back_button.width()/2)))
		{
			if((first_touch.position.y > (theGameState.game_back_button.GetY() - theGameState.game_back_button.height()/2)) && (first_touch.position.y < (theGameState.game_back_button.GetY() + theGameState.game_back_button.height()/2)))
			{
				next_state = &theMenuState;
			}
		}
	}

	//return to main menu after reading instructions
	if(AppCurrentState == &theHowtoplayState)
	{
		if(first_touch.type == abfw::TT_RELEASED)
		{
			if((first_touch.position.x > (theHowtoplayState.main_menu_button.GetX() - theHowtoplayState.main_menu_button.width()/2)) && (first_touch.position.x < (theHowtoplayState.main_menu_button.GetX() + theHowtoplayState.main_menu_button.width()/2)))
			{
				if((first_touch.position.y > (theHowtoplayState.main_menu_button.GetY() - theHowtoplayState.main_menu_button.height()/2)) && (first_touch.position.y < (theHowtoplayState.main_menu_button.GetY() + theHowtoplayState.main_menu_button.height()/2)))
				{
					next_state = &theMenuState;
				}
			}
		}
	}

	//select player options and start game
	if (AppCurrentState == &theOptionsState)
	{
		if(first_touch.type != abfw::TT_RELEASED)
		{
			if((first_touch.position.x > (theOptionsState.start_game_button.GetX() - theOptionsState.start_game_button.width()/2)) && (first_touch.position.x < (theOptionsState.start_game_button.GetX() + theOptionsState.start_game_button.width()/2)))
			{
				if((first_touch.position.y > (theOptionsState.start_game_button.GetY() - theOptionsState.start_game_button.height()/2)) && (first_touch.position.y < (theOptionsState.start_game_button.GetY() + theOptionsState.start_game_button.height()/2)))
				{
					theOptionsState.start_game_button.set_uv_position(abfw::Vector2(0.5f, 0.0f));

					next_state = &theGameState;
					theGameState.player_selection = theOptionsState.get_selection();
					theGameState.character_selection = theOptionsState.get_character();
				}
			}
		}
	}
}

