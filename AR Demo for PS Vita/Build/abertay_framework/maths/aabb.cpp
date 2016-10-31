#include <maths/aabb.h>
#include <abertay_framework.h>

namespace abfw
{
	Aabb::Aabb() :
		initialised_(false)
	{
	}

	Aabb::Aabb(const Vector3& min_vtx, const Vector3& max_vtx)
	{
		min_vtx_ = min_vtx;
		max_vtx_ = max_vtx;
		initialised_ = true;
	}

	void Aabb::Update(const Vector3& point)
	{
		if(!initialised_)
		{
			min_vtx_ = point;
			max_vtx_ = point;
			initialised_ = true;
		}
		else
		{
			if(point.x < min_vtx_.x)
				min_vtx_.x = point.x;
			if(point.y < min_vtx_.y)
				min_vtx_.y = point.y;
			if(point.z < min_vtx_.z)
				min_vtx_.z = point.z;
			if(point.x > max_vtx_.x)
				max_vtx_.x = point.x;
			if(point.y > max_vtx_.y)
				max_vtx_.y = point.y;
			if(point.z > max_vtx_.z)
				max_vtx_.z = point.z;
		}
	}

	const Aabb Aabb::Transform(const Matrix44& transform_matrix) const
	{
		// This can be optimised
		Aabb result;

		abfw::Vector3 vertices[8];

		vertices[0].x = min_vtx_.x;
		vertices[0].y = min_vtx_.y;
		vertices[0].z = min_vtx_.z;
		vertices[1].x = max_vtx_.x;
		vertices[1].y = min_vtx_.y;
		vertices[1].z = min_vtx_.z;
		vertices[2].x = min_vtx_.x;
		vertices[2].y = max_vtx_.y;
		vertices[2].z = min_vtx_.z;
		vertices[3].x = max_vtx_.x;
		vertices[3].y = max_vtx_.y;
		vertices[3].z = min_vtx_.z;
		vertices[4].x = min_vtx_.x;
		vertices[4].y = min_vtx_.y;
		vertices[4].z = max_vtx_.z;
		vertices[5].x = max_vtx_.x;
		vertices[5].y = min_vtx_.y;
		vertices[5].z = max_vtx_.z;
		vertices[6].x = min_vtx_.x;
		vertices[6].y = max_vtx_.y;
		vertices[6].z = max_vtx_.z;
		vertices[7].x = max_vtx_.x;
		vertices[7].y = max_vtx_.y;
		vertices[7].z = max_vtx_.z;

		for(Int32 vertex_num=0; vertex_num < 8;++vertex_num)
			result.Update(vertices[vertex_num].Transform(transform_matrix));

		return result;
	}

}