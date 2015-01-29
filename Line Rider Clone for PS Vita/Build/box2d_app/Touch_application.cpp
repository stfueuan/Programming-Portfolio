#include "Touch_application.h"

using namespace std;

TouchApplication:: TouchApplication() : 
	touch_manager_(NULL)
{
}

TouchApplication::~TouchApplication()
{
}

void TouchApplication::Init(abfw::Platform* platform)
{
	touch_manager_ = platform->CreateTouchInputManager();
	touch_manager_->EnablePanel(0);
	touch_manager_->EnablePanel(1);
}

void TouchApplication::CleanUp()
{
   delete touch_manager_;
   touch_manager_ = NULL;
}

bool TouchApplication::Update()
{
	touch_manager_->Update();

	return true;
}



