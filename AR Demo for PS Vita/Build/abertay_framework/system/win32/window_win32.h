#ifndef _ABFW_WINDOW_WIN32_H
#define _ABFW_WINDOW_WIN32_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <abertay_framework.h>

namespace abfw
{
	class WindowWin32
	{
	public:
		WindowWin32(HINSTANCE hinstance, UInt32 screen_width, UInt32 screen_height, bool fullscreen);
		~WindowWin32();
		inline HWND hwnd() { return hwnd_; }
		inline UInt32 bits_per_pixel() { return bits_per_pixel_; }
		inline UInt32 width() { return width_; }
		inline UInt32 height() { return height_; }
	private:
		HWND hwnd_;
		HINSTANCE hinstance_;
		bool fullscreen_;
		LPCSTR application_name_;
		UInt32 bits_per_pixel_;
		UInt32 width_;
		UInt32 height_;
	};
}

#endif // _WINDOW_WIN32_H