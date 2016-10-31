#include <graphics/model.h>
#include <vector>
#include <maths/vector3.h>
#include <maths/vector2.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <system/platform.h>
#include <graphics/texture.h>


namespace abfw
{
	Model::Model() :
		mesh_(NULL)
	{
	}

	Model::~Model()
	{
		Release();
	}

	void Model::Release()
	{
		for(Int32 texture_num=0;texture_num<textures_.size();++texture_num)
			DeleteNull(textures_[texture_num]);
		textures_.clear();
		DeleteNull(mesh_);
	}
}