#include <assets/obj_loader.h>
#include <vector>
#include <maths/vector3.h>
#include <maths/vector2.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <system/platform.h>
#include <graphics/model.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <system/file.h>
#include <system/memory_stream_buffer.h>

#include <cstdio>

namespace abfw
{


	bool OBJLoader::Load(const char* filename, const Platform& platform, Model& model)
	{
		bool success = true;
		//	num_textures_ = 1;
		//	textures_ = new Texture*[num_textures_];
		//	textures_[0] = NULL;
		std::vector<Texture*> textures;

		std::vector<abfw::Vector3> positions;
		std::vector<abfw::Vector3> normals;
		std::vector<abfw::Vector2> uvs;
		std::vector<Int32> face_indices;
		std::vector<Int32> primitive_indices;
		std::vector<Int32> texture_indices;

		std::map<std::string, Int32> materials;

		std::string obj_filename(filename);
		void* obj_file_data = NULL;
		Int32 file_size = 0;
		abfw::File* file = platform.CreateFile();

		//bool success = true;
		success = file->Open(obj_filename.c_str());
		if (success)
		{
			success = file->GetSize(file_size);
			if (success)
			{
				obj_file_data = malloc(file_size);
				success = obj_file_data != NULL;
				if (success)
				{
					Int32 bytes_read;
					success = file->Read(obj_file_data, file_size, bytes_read);
					if (success)
						success = bytes_read == file_size;
				}
			}
		}

		if (!success)
		{
			free(obj_file_data);
			obj_file_data = NULL;

			delete file;
			file = NULL;
			return false;
		}
		abfw::MemoryStreamBuffer buffer((char*)obj_file_data, file_size);
		std::istream stream(&buffer);

		{
			while (!stream.eof())
			{
				char line[128];
				stream >> line;
				if (strcmp(line, "mtllib") == 0)
				{
					char material_filename[256];
					stream >> material_filename;
					//				fscanf(file, "%s\n", material_filename );
					//				LoadMaterials(material_filename, materials);
					LoadMaterials(platform, material_filename, materials, textures);
				}

				// vertices
				else if (strcmp(line, "v") == 0)
				{
					float x, y, z;
					stream >> x;
					stream >> y;
					stream >> z;
					positions.push_back(abfw::Vector3(x, y, z));
				}

				// normals
				else if (strcmp(line, "vn") == 0)
				{
					float nx, ny, nz;
					stream >> nx;
					stream >> ny;
					stream >> nz;
					normals.push_back(abfw::Vector3(nx, ny, nz));
				}

				// uvs
				else if (strcmp(line, "vt") == 0)
				{
					float u, v;
					stream >> u;
					stream >> v;
					uvs.push_back(abfw::Vector2(u, v));
				}

				else if (strcmp(line, "usemtl") == 0)
				{

					char material_name[256];
					stream >> material_name;

					// any time the material is changed
					// a new primitive is created
					primitive_indices.push_back(face_indices.size());

					texture_indices.push_back(materials[material_name]);
				}
				else if (strcmp(line, "f") == 0)
				{
					Int32 vertexIndex[3], uvIndex[3], normalIndex[3];
					/*
					//std::string vertex1, vertex2, vertex3;
					Int32 matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
					if (matches != 9)
					{
					//					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					success = false;
					}
					*/

					stream >> vertexIndex[0]; stream.ignore(); stream >> uvIndex[0]; stream.ignore(); stream >> normalIndex[0];
					stream >> vertexIndex[1]; stream.ignore(); stream >> uvIndex[1]; stream.ignore(); stream >> normalIndex[1];
					stream >> vertexIndex[2]; stream.ignore(); stream >> uvIndex[2]; stream.ignore(); stream >> normalIndex[2];

					face_indices.push_back(vertexIndex[2]);
					face_indices.push_back(uvIndex[2]);
					face_indices.push_back(normalIndex[2]);

					face_indices.push_back(vertexIndex[1]);
					face_indices.push_back(uvIndex[1]);
					face_indices.push_back(normalIndex[1]);

					face_indices.push_back(vertexIndex[0]);
					face_indices.push_back(uvIndex[0]);
					face_indices.push_back(normalIndex[0]);
				}
			}

			// don't need the font file data any more
			free(obj_file_data);
			obj_file_data = NULL;

			// finished reading the file
			//		fclose(file);
			// start building the mesh
			Int32 num_faces = face_indices.size() / 9;
			Int32 num_vertices = num_faces * 3;


			abfw::Aabb aabb;

			// create vertex buffer
			abfw::Mesh::Vertex* vertices = new abfw::Mesh::Vertex[num_vertices];
			for (Int32 vertex_num = 0; vertex_num < num_vertices; ++vertex_num)
			{
				abfw::Mesh::Vertex* vertex = &vertices[vertex_num];
				abfw::Vector3 position = positions[face_indices[vertex_num * 3] - 1];
				abfw::Vector2 uv = uvs[face_indices[vertex_num * 3 + 1] - 1];
				abfw::Vector3 normal = normals[face_indices[vertex_num * 3 + 2] - 1];
				vertex->position = position;
				vertex->normal = normal;
				vertex->uv.x = uv.x;
				vertex->uv.y = -uv.y;

				aabb.Update(position);
			}
			abfw::Sphere bounding_sphere(aabb);

			Mesh* mesh = platform.CreateMesh();
			model.set_mesh(mesh);
			model.set_textures(textures);
			mesh->InitVertexBuffer(platform, vertices, num_vertices, sizeof(abfw::Mesh::Vertex));
			mesh->set_aabb(aabb);
			mesh->set_bounding_sphere(bounding_sphere);

			// create primitives
			mesh->AllocatePrimitives(primitive_indices.size());

			std::vector<UInt32*> indices;
			indices.resize(primitive_indices.size());
			for (Int32 primitive_num = 0; primitive_num<primitive_indices.size(); ++primitive_num)
			{
				Int32 index_count = 0;

				if (primitive_num == primitive_indices.size() - 1)
					index_count = face_indices.size() - primitive_indices[primitive_num];
				else
					index_count = primitive_indices[primitive_num + 1] - primitive_indices[primitive_num];

				// 9 indices per triangle, index count is the number of vertices in this primitive
				index_count /= 3;

				indices[primitive_num] = new UInt32[index_count];

				for (Int32 index = 0; index<index_count; ++index)
					indices[primitive_num][index] = primitive_indices[primitive_num] + index;

				mesh->GetPrimitive(primitive_num)->SetType(abfw::TRIANGLE_LIST);
				mesh->GetPrimitive(primitive_num)->InitIndexBuffer(platform, indices[primitive_num], index_count, sizeof(UInt32));
				//			mesh->GetPrimitive(primitive_num)->InitIndexBuffer(platform, indices[primitive_num], 3, sizeof(UInt32));


				Int32 texture_index = texture_indices[primitive_num];
				if (texture_index == -1)
					mesh->GetPrimitive(primitive_num)->set_texture(NULL);
				else
					mesh->GetPrimitive(primitive_num)->set_texture(textures[texture_index]);
				//mesh->GetPrimitive(primitive_num)->set_texture(textures_[0]);
			}

			// mesh construction complete
			// clean up
			DeleteArrayNull(vertices);
			for (Int32 primitive_num = 0; primitive_num<indices.size(); ++primitive_num)
				DeleteArrayNull(indices[primitive_num]);

		}
		return success;
	}

	bool OBJLoader::LoadMaterials(const Platform& platform, const char* filename, std::map<std::string, Int32>& materials, std::vector<Texture*>& textures)
	{
		PNGLoader png_loader;

		bool success = true;


		std::string mtl_filename(filename);
		void* mtl_file_data = NULL;
		Int32 file_size = 0;
		abfw::File* file = platform.CreateFile();
		success = file->Open(mtl_filename.c_str());
		if (success)
		{
			success = file->GetSize(file_size);
			if (success)
			{
				mtl_file_data = malloc(file_size);
				success = mtl_file_data != NULL;
				if (success)
				{
					Int32 bytes_read;
					success = file->Read(mtl_file_data, file_size, bytes_read);
					if (success)
						success = bytes_read == file_size;
				}
			}
		}

		if (!success)
		{
			free(mtl_file_data);
			mtl_file_data = NULL;

			delete file;
			file = NULL;
			return false;
		}
		abfw::MemoryStreamBuffer buffer((char*)mtl_file_data, file_size);
		std::istream stream(&buffer);


		{
			std::map<std::string, std::string> material_name_mappings;
			char material_name[256];
			while (!stream.eof())
			{
				char line[128];
				stream >> line;

				if (strcmp(line, "newmtl") == 0)
				{
					stream >> material_name;
					material_name_mappings[material_name] = "";
				}
				else if (strcmp(line, "map_Kd") == 0)
				{
					char texture_name[256];
					stream >> texture_name;
					material_name_mappings[material_name] = texture_name;
				}
			}

			free(mtl_file_data);
			mtl_file_data = NULL;

			for (std::map<std::string, std::string>::iterator iter = material_name_mappings.begin(); iter != material_name_mappings.end(); ++iter)
			{
				if (iter->second.compare("") != 0)
				{
					abfw::ImageData image_data;
					png_loader.Load(iter->second.c_str(), platform, image_data);
					Texture* texture = platform.CreateTexture(image_data);
					textures.push_back(texture);
					materials[iter->first] = textures.size() - 1;
				}
				else
				{
					materials[iter->first] = -1;
				}
			}
		}

		return success;

	}

}