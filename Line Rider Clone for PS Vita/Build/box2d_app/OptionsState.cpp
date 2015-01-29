#include "OptionsState.h"

OptionsState::OptionsState(abfw::Platform* platform) 
{
	platform_ = platform;
}

OptionsState::~OptionsState()
{
}

void OptionsState::Enter(abfw::AudioManagerVita* audio_manager_)
{
	Draw(); //initialise all sprites on screen

	Select = circle_player; //give the player a default value
}

State* OptionsState::Update(abfw::TouchInputManager* touch_manager_, abfw::ControllerInputManagerVita& controller_manager_, abfw::AudioManagerVita* audio_manager_, float ticks)
{
	Touch_input(touch_manager_);

	return this;
}

void OptionsState::Draw()
{
	float button_width = 100;
	float button_height = 100;

	options_screen.set_height(platform_->height());
	options_screen.set_width(platform_->width());
	options_screen.set_position(abfw::Vector3(platform_->width()/2, platform_->height()/2, 0.0f));

	choose_ball.set_height(button_height);
	choose_ball.set_width(button_width);
	choose_ball.set_position(abfw::Vector3(platform_->width()/2 - 150, (platform_->height()*2)/5, 0.0f));
	choose_ball.set_uv_width(0.5f);
	choose_ball.set_uv_height(0.25f);
	choose_ball.set_uv_position(abfw::Vector2(0.0f, 0.75f));

	choose_car.set_height(button_height);
	choose_car.set_width(button_width);
	choose_car.set_position(abfw::Vector3(platform_->width()/2 + 150, (platform_->height()*2)/5, 0.0f));
	choose_car.set_uv_width(0.5f);
	choose_car.set_uv_height(0.25f);
	choose_car.set_uv_position(abfw::Vector2(0.0f, 0.5f));

	choose_penguin.set_height(button_height);
	choose_penguin.set_width(button_width);
	choose_penguin.set_position(abfw::Vector3(platform_->width()/2 - 150, (platform_->height()*3)/5, 0.0f));
	choose_penguin.set_uv_width(0.5f);
	choose_penguin.set_uv_height(0.25f);
	choose_penguin.set_uv_position(abfw::Vector2(0.0f, 0.0f));

	choose_mouse.set_height(button_height);
	choose_mouse.set_width(button_width);
	choose_mouse.set_position(abfw::Vector3(platform_->width()/2 + 150, (platform_->height()*3)/5, 0.0f));
	choose_mouse.set_uv_width(0.5f);
	choose_mouse.set_uv_height(0.25f);
	choose_mouse.set_uv_position(abfw::Vector2(0.0f, 0.25f));

	start_game_button.set_height(92.0f);
	start_game_button.set_width(384.0f);
	start_game_button.set_position(abfw::Vector3(platform_->width()/2, 450.0f, 0.0f));
	start_game_button.set_uv_height(1.0f/4.0f);
	start_game_button.set_uv_width(1.0f/2.0f);
	start_game_button.set_uv_position(abfw::Vector2(0.0f, 0.0f));
}

void OptionsState::Exit(abfw::AudioManagerVita* audio_manager_)
{
}

void OptionsState::Controller_input(abfw::ControllerInputManagerVita controller_manager_)
{
}

void OptionsState::Touch_input(abfw::TouchInputManager* touch_manager_)
{
	const abfw::Touch& first_touch = touch_manager_->GetTouch(0, 0);
	
	//player makes their choices
	if(first_touch.type != abfw::TT_NONE)
	{
		if((first_touch.position.x > (choose_ball.GetX() - choose_ball.width()/2)) && (first_touch.position.x < (choose_ball.GetX() + choose_ball.width()/2)))
		{
			if((first_touch.position.y > (choose_ball.GetY() - choose_ball.height()/2)) && (first_touch.position.y < (choose_ball.GetY() + choose_ball.height()/2)))
			{
				Select = circle_player;
				choose_ball.set_uv_position(abfw::Vector2(0.5f, 0.75f));
				choose_car.set_uv_position(abfw::Vector2(0.0f, 0.5f));
			}
		}

		if((first_touch.position.x > (choose_car.GetX() - choose_car.width()/2)) && (first_touch.position.x < (choose_car.GetX() + choose_car.width()/2)))
		{
			if((first_touch.position.y > (choose_car.GetY() - choose_car.height()/2)) && (first_touch.position.y < (choose_car.GetY() + choose_car.height()/2)))
			{
				Select = car_player;
				choose_car.set_uv_position(abfw::Vector2(0.5f, 0.5f));
				choose_ball.set_uv_position(abfw::Vector2(0.0f, 0.75f));
			}
		}

		if((first_touch.position.x > (choose_penguin.GetX() - choose_penguin.width()/2)) && (first_touch.position.x < (choose_penguin.GetX() + choose_penguin.width()/2)))
		{
			if((first_touch.position.y > (choose_penguin.GetY() - choose_penguin.height()/2)) && (first_touch.position.y < (choose_penguin.GetY() + choose_penguin.height()/2)))
			{
				Character = penguin_character;
				choose_penguin.set_uv_position(abfw::Vector2(0.5f, 0.0f));
				choose_mouse.set_uv_position(abfw::Vector2(0.0f, 0.25f));
			}
		}

		if((first_touch.position.x > (choose_mouse.GetX() - choose_mouse.width()/2)) && (first_touch.position.x < (choose_mouse.GetX() + choose_mouse.width()/2)))
		{
			if((first_touch.position.y > (choose_mouse.GetY() - choose_mouse.height()/2)) && (first_touch.position.y < (choose_mouse.GetY() + choose_mouse.height()/2)))
			{
				Character = mouse_character;
				choose_mouse.set_uv_position(abfw::Vector2(0.5f, 0.25f));
				choose_penguin.set_uv_position(abfw::Vector2(0.0f, 0.0f));
			}
		}
	}
}