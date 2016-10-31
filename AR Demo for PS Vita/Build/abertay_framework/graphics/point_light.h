#ifndef _ABFW_POINT_LIGHT_H
#define _ABFW_POINT_LIGHT_H

#include <graphics/colour.h>
#include <maths/vector3.h>

namespace abfw
{
	class PointLight
	{
	public:
		const Colour& colour() const { return colour_; }
		void set_colour(const Colour& colour) { colour_ = colour; }
		const Vector3& position() const { return position_; }
		void set_position(const Vector3& position) { position_ = position; }
	private:
		Colour colour_;
		Vector3 position_;
	};
}

#endif // _ABFW_POINT_LIGHT_H