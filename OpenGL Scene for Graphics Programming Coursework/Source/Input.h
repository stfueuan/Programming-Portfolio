#ifndef INPUT_H
#define INPUT_H

class Input
{
	typedef struct Mouse
	{
		int x, y;
		bool left, right, middle, doubleclick;
	};

public:
	// getters and setters prototypes
	void SetKeyDown(WPARAM);
	void SetKeyUp(WPARAM);
	bool isMouseDown();
	bool isMouseUp();
	bool isMouseClicked();

	bool isKeyDown(int);
	void LMouseClick();
	void LMouseUp();
	void RMouseClick();
	void RMouseUp();
	void MMouseClick();
	void DBMouseClick();
	int setMouseX(int);
	int setMouseY(int);
	int getMouseX();
	int getMouseY();

	bool isLmousedown();
	


private:
	bool keys[256];	//Boolean array for key presses
	Mouse mouse;	// mouse struct
};


#endif