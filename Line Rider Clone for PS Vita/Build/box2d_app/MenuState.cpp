#include "MenuState.h"

MenuState::MenuState(abfw::Platform* platform) 
{
	platform_ = platform;
}

MenuState::~MenuState()
{
}

void MenuState::Enter(abfw::AudioManagerVita* audio_manager_)
{
	bing_sound = audio_manager_->LoadSample("91926__corsica-s__ding.wav", *platform_);
	menu_music = audio_manager_->LoadSample("bensound-popdance.wav", *platform_);

	Draw(); //initialise all sprites on screen
}

State* MenuState::Update(abfw::TouchInputManager* touch_manager_, abfw::ControllerInputManagerVita& controller_manager_, abfw::AudioManagerVita* audio_manager_, float ticks)
{
	if(menu_music != -1)
	{
		audio_manager_->PlaySample(menu_music, true);
	}

	return this;
}

void MenuState::Draw()
{
	float button_width = 384.0f;
	float button_height = 92.0f;
	float button_centre_x = platform_->width()/2;

	title_screen.set_height(platform_->height());
	title_screen.set_width(platform_->width());
	title_screen.set_position(abfw::Vector3((platform_->width()/2), (platform_->height()/2), 0));
	
	start_game.set_height(button_height);
	start_game.set_width(button_width);
	start_game.set_position(abfw::Vector3(button_centre_x, 226, -0.1f));
	start_game.set_uv_height(1.0f/4.0f);
	start_game.set_uv_width(1.0f/2.0f);
	start_game.set_uv_position(abfw::Vector2(0.0f, 0.0f));

	options.set_height(button_height);
	options.set_width(button_width);
	options.set_position(abfw::Vector3(button_centre_x, 336, -0.1f));
	options.set_uv_height(1.0f/4.0f);
	options.set_uv_width(1.0f/2.0f);
	options.set_uv_position(abfw::Vector2(0.0f, (1.0f/4.0f)));

	credits.set_height(button_height);
	credits.set_width(button_width);
	credits.set_position(abfw::Vector3(button_centre_x, 446, -0.1f));
	credits.set_uv_height(1.0f/4.0f);
	credits.set_uv_width(1.0f/2.0f);
	credits.set_uv_position(abfw::Vector2(0.0f, (1.0f/2.0f)));
}

void MenuState::Exit(abfw::AudioManagerVita* audio_manager_)
{
	audio_manager_->UnloadSample(bing_sound);
	audio_manager_->UnloadSample(menu_music);
}

void MenuState::Controller_input(abfw::ControllerInputManagerVita controller_manager_)
{
}

void MenuState::Touch_input(abfw::TouchInputManager* touch_manager_)
{
}