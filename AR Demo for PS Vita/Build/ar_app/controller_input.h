#ifndef CONTROLLER_INPUT_H
#define CONTROLLER_INPUT_H

#include <system/platform.h>
#include <input\sony_controller_input_manager.h>
#include "gameObject.h"
#include <math.h>
#include "LiveFeed.h"

namespace abfw
{
	class Platform;
	class SonyControllerInputManager;
}

class ControllerInput
{
public:
	ControllerInput(abfw::Platform& platform, GameObject& gameObject, LiveFeed& liveFeed);
	~ControllerInput();

	void Init();
	void Update(float frameTime);

private:
	abfw::SonyControllerInputManager* controller_input_manager_;
	const abfw::SonyController* controller;
	abfw::Platform* platform_;
	GameObject* gameObject_;
	LiveFeed* liveFeed_;

	float movementAmount;
	float angle, prevAngle;
};

#endif //CONTROLLER_INPUT_H