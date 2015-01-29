#include "HowToPlayState.h"

HowToPlayState::HowToPlayState(abfw::Platform* platform) 
{
	platform_ = platform;
}

HowToPlayState::~HowToPlayState()
{
}

void HowToPlayState::Enter(abfw::AudioManagerVita* audio_manager_)
{
}

State* HowToPlayState::Update(abfw::TouchInputManager* touch_manager_, abfw::ControllerInputManagerVita& controller_manager_, abfw::AudioManagerVita* audio_manager_, float ticks)
{
	Draw(); //initialise all sprites on screen

	return this;
}

void HowToPlayState::Draw()
{
	howtoplay_background.set_height(platform_->height());
	howtoplay_background.set_width(platform_->width());
	howtoplay_background.set_position(abfw::Vector3(platform_->width()/2, platform_->height()/2, 0.0f));

	main_menu_button.set_height(92.0f);
	main_menu_button.set_width(384.0f);
	main_menu_button.set_position(abfw::Vector3(platform_->width()/2, 450.0f, 0.0f));
	main_menu_button.set_uv_height(1.0f/4.0f);
	main_menu_button.set_uv_width(0.5f);
	main_menu_button.set_uv_position(abfw::Vector2(0.0f, (3.0f/4.0f)));
}

void HowToPlayState::Exit(abfw::AudioManagerVita* audio_manager_)
{
}

void HowToPlayState::Controller_input(abfw::ControllerInputManagerVita controller_manager_)
{
}

void HowToPlayState::Touch_input(abfw::TouchInputManager* touch_manager_)
{
	const abfw::Touch& first_touch = touch_manager_->GetTouch(0, 0);

	if((first_touch.position.x > (main_menu_button.GetX() - main_menu_button.width()/2)) && (first_touch.position.x < (main_menu_button.GetX() + main_menu_button.width()/2)))
	{
		if((first_touch.position.y > (main_menu_button.GetY() - main_menu_button.height()/2)) && (first_touch.position.y < (main_menu_button.GetY() + main_menu_button.height()/2)))
		{
			main_menu_button.set_uv_position(abfw::Vector2(0.5f, (3.0f/4.0f)));
		}
	}
}