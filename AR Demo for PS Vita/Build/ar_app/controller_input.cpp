#include "controller_input.h"
#include <iostream>

ControllerInput::ControllerInput(abfw::Platform& platform, GameObject& gameObject, LiveFeed& liveFeed)
{
	platform_ = &platform;
	gameObject_ = &gameObject;
	liveFeed_ = &liveFeed;
}

ControllerInput::~ControllerInput()
{
	delete platform_;
	platform_ = 0;

	if (gameObject_)
	{
		delete gameObject_;
		gameObject_ = NULL;
	}

	if (liveFeed_)
	{
		delete liveFeed_;
		liveFeed_ = NULL;
	}
}

void ControllerInput::Init()
{
	controller_input_manager_ = platform_->CreateSonyControllerInputManager();

	//How much to translate the car by when accelerating
	movementAmount = 0.01f;
	prevAngle = 0.0f;
}

void ControllerInput::Update(float frameTime)
{
	if (controller_input_manager_)
	{
		controller_input_manager_->Update();
	}

	controller = controller_input_manager_->GetController(0);

	//make sure the gameobject actually exists
	if (gameObject_)
	{
		//This switch controls whether the buttons are being held down
		switch (controller->buttons_down())
		{
		case ABFW_SONY_CTRL_CROSS:
			gameObject_->Accelerate(frameTime);
			break;
		case ABFW_SONY_CTRL_SQUARE:
			gameObject_->Decelerate(frameTime, true);
			break;
		case ABFW_SONY_CTRL_LEFT:
			gameObject_->Move(abfw::Vector3(-movementAmount, 0.0f, 0.0f));
			break;
		case ABFW_SONY_CTRL_RIGHT:
			gameObject_->Move(abfw::Vector3(movementAmount, 0.0f, 0.0f));
			break;
		case ABFW_SONY_CTRL_UP:
			gameObject_->Move(abfw::Vector3(0.0f, movementAmount, 0.0f));
			break;
		case ABFW_SONY_CTRL_DOWN:
			gameObject_->Move(abfw::Vector3(0.0f, -movementAmount, 0.0f));
			break;
		}
	}

	//This switch determines if the buttons have been pressed during a single frame
	switch (controller->buttons_pressed())
	{
	case ABFW_SONY_CTRL_CIRCLE:
		gameObject_->Reset(gameObject_->GetInitialPos());
		break;
	case ABFW_SONY_CTRL_TRIANGLE:
		//This resets the camera feed upon user request (usually when too bright)
		if (liveFeed_)
			liveFeed_->Reset();
		break;
	case ABFW_SONY_CTRL_L1:
		break;
	case ABFW_SONY_CTRL_R1:
		break;
	case ABFW_SONY_CTRL_START:
		break;
	case ABFW_SONY_CTRL_SELECT:
		break;
	}

	//If the gameobject exists, in this case the car, get the desired angle of rotation from the left analog stick
	if (gameObject_)
	{
		//Only allow the player to steer the car if it is actually in motion
		if (gameObject_->GetVelocity().Length() > 0.0f)
		{
			//Stop the car spinning when the player is not touching the left analog stick
			if (controller->left_stick_x_axis() != 0.0f && controller->left_stick_x_axis() != 0.0f)
			{
				angle = atan2f(-controller->left_stick_x_axis(), -controller->left_stick_y_axis());
			}
			else
			{
				angle = 0.0f;
			}

			gameObject_->RotateLocalZ(angle * frameTime);
			
			//If the player is not accelerating, decelerate
			if (!(controller->buttons_down() && ABFW_SONY_CTRL_CROSS))
			{
				gameObject_->Decelerate(frameTime);
			}
		}
	}
}
