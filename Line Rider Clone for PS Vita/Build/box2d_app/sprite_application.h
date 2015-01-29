#ifndef _SPRITE_APPLICATION_H
#define _SPRITE_APPLICATION_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <graphics/font.h>
#include <graphics/sprite_renderer.h>
#include <input/touch_input_manager.h>
#include <input/vita/controller_input_manager_vita.h>
#include <audio/vita/audio_manager_vita.h>
#include <system/platform.h>
#include <system/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <iostream>
#include "box2d_helpers.h"
#include "GameState.h"
#include "MenuState.h"
#include "State.h"
#include "OptionsState.h"
#include "HowToPlayState.h"

namespace abfw
{
	class Platform;
	class Texture;
	class TouchInputManager;
}

class SpriteApplication : public abfw::Application
{
public:
	SpriteApplication(abfw::Platform* platform);
	~SpriteApplication();
	void Init();
	void CleanUp();
	bool Update(float ticks);
	void Render();
	void Controller_input();
	void Touch_input();

private:
	abfw::Font font_;
	abfw::SpriteRenderer* sprite_renderer_;
	abfw::Texture* load_texture(const char *texture);
	abfw::ControllerInputManagerVita controller_manager_;
	abfw::TouchInputManager* touch_manager_;
	abfw::AudioManagerVita* audio_manager_;
	float frame_rate_;

	State* next_state;
	State* AppCurrentState;
	GameState theGameState;
	MenuState theMenuState;
	OptionsState theOptionsState;
	HowToPlayState theHowtoplayState;

	abfw::PNGLoader png_loader;
	abfw::ImageData image_data;
	abfw::Texture* player_texture;
	abfw::Texture* player_texture2;
	abfw::Texture* button_texture;
	abfw::Texture* title_texture;
	abfw::Texture* game_button_texture;
	abfw::Texture* game_background_texture;
	abfw::Texture* options_background_texture;
	abfw::Texture* howtoplay_background_texture;
	abfw::Texture* car_texture;
	abfw::Texture* options_button_texture;
};

#endif // _SPRITE_APPLICATION_H

