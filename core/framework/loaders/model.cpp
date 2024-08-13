#include "model.h"

#include <iostream>
#include <fstream>

namespace framework
{
	Model::Model(const std::string& path) : ModelBase(path)
	{
		Load(path);
	}

	bool Model::Load(const std::string& emlPath)
	{
		filename = emlPath;

		std::ifstream inFILE(emlPath, std::ios::binary);
		if (!inFILE.is_open())
		{
			std::cout << "Fail to open EML file" << std::endl;
			return false;
		}

		int numvertices = 0;
		int numMeshes = 0;
		int numIndices = 0;

		EML::t_eml_header* header = new EML::t_eml_header();
		EML::t_Lumps* lumps = new EML::t_Lumps[EML::maxLumps];

		inFILE.seekg(0, inFILE.beg);
		inFILE.read((char*)&header[0], sizeof(EML::t_eml_header));

		if ((int)header->format != EMLHEADER)
		{
			std::cout << "This file is not a valid SEML file. " << std::endl;
			return false;
		}

		if ((float)header->version != EML::emlVersion)
		{
			std::cout << "This file version doesn't match the one required." << std::endl
				<< "This file: " << (float)header->version << " Required: " << EML::emlVersion << std::endl;
			return false;
		}

		//Read the lump of this file
		inFILE.seekg(sizeof(EML::t_eml_header), inFILE.beg);
		for (int i = 0; i < EML::maxLumps; ++i)
		{
			inFILE.read((char*)&lumps[i], sizeof(EML::t_Lumps));
		}

		//Get the array of vertices in the file
		numvertices = lumps[EML::lVertices].size / sizeof(EML::t_Vertex);
		EML::t_Vertex* l_vertices = new EML::t_Vertex[numvertices];
		inFILE.seekg(lumps[EML::lVertices].offset, inFILE.beg);
		for (int i = 0; i < numvertices; ++i)
		{
			inFILE.read((char*)&l_vertices[i], sizeof(EML::t_Vertex));
		}

		//Get the array of indices in the file
		numIndices = lumps[EML::lIndices].size / sizeof(EML::t_Index);
		EML::t_Index* l_indices = new EML::t_Index[numIndices];
		inFILE.seekg(lumps[EML::lIndices].offset, inFILE.beg);
		for (int i = 0; i < numIndices; ++i)
		{
			inFILE.read((char*)&l_indices[i], sizeof(EML::t_Index));
		}

		//Get the array of meshes in the file
		numMeshes = (int)header->numOfMeshes;
		EML::t_Mesh* l_meshes = new EML::t_Mesh[numMeshes];
		inFILE.seekg(lumps[EML::lMeshes].offset, inFILE.beg);
		for (int i = 0; i < numMeshes; ++i)
		{
			inFILE.read((char*)&l_meshes[i], sizeof(EML::t_Mesh));
		}

		inFILE.close();
		std::cout << "Model " << emlPath << " has " << numMeshes << " meshes" << std::endl;

		MinMaxPoints.MAX_X = 0;
		MinMaxPoints.MAX_Y = 0;
		MinMaxPoints.MAX_Z = 0;

		MinMaxPoints.MIN_X = 0;
		MinMaxPoints.MIN_Y = 0;
		MinMaxPoints.MIN_Z = 0;

		//Pre Process the model and create it's bounding boxes
		unsigned currentvOffset = 0;
		unsigned currentiOffset = 0;
		try
		{

			for (int i = 0; i < numMeshes; ++i)
			{
				//mMeshesNames[i] = std::to_string(i);
				currentvOffset += l_meshes[i].mNumVertices;
				currentiOffset += l_meshes[i].mNumIndices;

				std::vector<EML::t_Vertex> tmpVertVector;
				std::vector<unsigned int> tmpIndicesVector;
				Mesh::MeshMaterial meshMaterial;

				common::MIN_MAX_POINTS tmp_MinMax;

				for (int j = 0; j < l_meshes[i].mNumVertices; ++j)
				{
					tmpVertVector.push_back(l_vertices[l_meshes[i].mFirstVertex + j]);

					if (l_vertices[l_meshes[i].mFirstVertex + j].position.x > MinMaxPoints.MAX_X)
						MinMaxPoints.MAX_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.y > MinMaxPoints.MAX_Y)
						MinMaxPoints.MAX_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.z > MinMaxPoints.MAX_Z)
						MinMaxPoints.MAX_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;

					if (l_vertices[l_meshes[i].mFirstVertex + j].position.x < MinMaxPoints.MIN_X)
						MinMaxPoints.MIN_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.y < MinMaxPoints.MIN_Y)
						MinMaxPoints.MIN_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.z < MinMaxPoints.MIN_Z)
						MinMaxPoints.MIN_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;

					//min max for this mesh

					if (l_vertices[l_meshes[i].mFirstVertex + j].position.x > tmp_MinMax.MAX_X)
						tmp_MinMax.MAX_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.y > tmp_MinMax.MAX_Y)
						tmp_MinMax.MAX_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.z > tmp_MinMax.MAX_Z)
						tmp_MinMax.MAX_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;

					if (l_vertices[l_meshes[i].mFirstVertex + j].position.x < tmp_MinMax.MIN_X)
						tmp_MinMax.MIN_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.y < tmp_MinMax.MIN_Y)
						tmp_MinMax.MIN_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
					if (l_vertices[l_meshes[i].mFirstVertex + j].position.z < tmp_MinMax.MIN_Z)
						tmp_MinMax.MIN_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;
				}

				float dx, dy, dz;
				dx = (tmp_MinMax.MAX_X - tmp_MinMax.MIN_X);
				dy = (tmp_MinMax.MAX_Y - tmp_MinMax.MIN_Y);
				dz = (tmp_MinMax.MAX_Z - tmp_MinMax.MIN_Z);

				glm::vec3 centerOfMass = glm::vec3(tmp_MinMax.MIN_X + (dx * 0.5),
												   tmp_MinMax.MIN_Y + (dy * 0.5),
												   tmp_MinMax.MIN_Z + (dz * 0.5));

				glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

				glm::vec3 delta = origin - centerOfMass;

				//mMeshesBoundingBoxes.push_back(tmp_MinMax);

				//mMeshesPositionsRelative.push_back(centerOfMass);
				//mMeshesPositions.push_back(centerOfMass);

				for (int j = 0; j < l_meshes[i].mNumIndices; ++j)
				{
					tmpIndicesVector.push_back(l_indices[l_meshes[i].mFirstIndex + j].index);
				}
				for (int j = 0; j < 4; ++j)
				{
					switch (j) {
						case EML::MATERIAL_COMPONENT::Albedo:
							meshMaterial.albedo = std::string(l_meshes[i].mMaterial[EML::MATERIAL_COMPONENT::Albedo]);
							break;
						case EML::MATERIAL_COMPONENT::Roughness:
							meshMaterial.roughness = std::string(l_meshes[i].mMaterial[EML::MATERIAL_COMPONENT::Roughness]);
							break;
						case EML::MATERIAL_COMPONENT::Normal:
							meshMaterial.normal = std::string(l_meshes[i].mMaterial[EML::MATERIAL_COMPONENT::Normal]);
							break;
						case EML::MATERIAL_COMPONENT::Metallic:
							meshMaterial.metallic = std::string(l_meshes[i].mMaterial[EML::MATERIAL_COMPONENT::Metallic]);
							break;
					}
				}


				framework::Mesh currentMesh(tmpVertVector, tmpIndicesVector, meshMaterial, currentvOffset, currentiOffset);
				currentMesh.setMinMaxPoints(tmp_MinMax);
				currentMesh.setRelativePosition(centerOfMass);

				mMeshes.push_back(currentMesh);

				//mMeshes.back().mLocalTransform = glm::translate(glm::mat4(0.0f), centerOfMass);
				//mMeshes.back().MinMaxPoints = tmp_MinMax;
			}

			float dx, dy, dz;
			dx = (MinMaxPoints.MAX_X - MinMaxPoints.MIN_X);
			dy = (MinMaxPoints.MAX_Y - MinMaxPoints.MIN_Y);
			dz = (MinMaxPoints.MAX_Z - MinMaxPoints.MIN_Z);

			glm::vec3 centerOfMass = glm::vec3(MinMaxPoints.MIN_X + (dx * 0.5),
				MinMaxPoints.MIN_Y + (dy * 0.5),
				MinMaxPoints.MIN_Z + (dz * 0.5));

			glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

			glm::vec3 delta = origin - centerOfMass;

			MinMaxPoints.MIN_X += delta.x;
			MinMaxPoints.MIN_Y += delta.y;
			MinMaxPoints.MIN_Z += delta.z;

			MinMaxPoints.MAX_X += delta.x;
			MinMaxPoints.MAX_Y += delta.y;
			MinMaxPoints.MAX_Z += delta.z;

			for (auto &mesh: mMeshes)
			{
				/*for (int j = 0; j < mMeshes.at(i).data().mesh.Vertices.size(); j++)
				{
					mMeshes.at(i).vertices[j].position.x += delta.x;
					mMeshes.at(i).vertices[j].position.y += delta.y;
					mMeshes.at(i).vertices[j].position.z += delta.z;
				}*/
				common::MIN_MAX_POINTS points = mesh.getMinMaxPoints();

				points.MIN_X += delta.x;
				points.MIN_Y += delta.y;
				points.MIN_Z += delta.z;

				points.MAX_X += delta.x;
				points.MAX_Y += delta.y;
				points.MAX_Z += delta.z;

				mesh.setMinMaxPoints(points);
			}
		}

		catch (std::exception& e)
		{
			throw std::runtime_error(std::string("Exception while pre processing the model ::: ") + std::string(e.what()));
		}

		delete header;
		delete[] lumps;
		delete[] l_vertices;
		delete[] l_indices;
		delete[] l_meshes;
		//std::cout << "llega model" << std::endl;

		//Log::WriteToLog("Model: " + std::string(emlPath) + " Loaded.");

		return true;
	}
	/*
	MIN_MAX_POINTS Model::getMeshBoundingBox(unsigned int index, glm::vec3 position, glm::vec3 scale, glm::quat rotation)
	{
		if (index > mMeshes.size() - 1)
		{
			std::cerr << "Index for mesh bounding box is out of bounds\n";
			return MIN_MAX_POINTS{};
		}
		MIN_MAX_POINTS BB = mMeshes.at(index).MinMaxPoints;
		BB.MAX_X = (BB.MAX_X * scale.x) + position.x;
		BB.MAX_Y = (BB.MAX_Y * scale.y) + position.y;
		BB.MAX_Z = (BB.MAX_Z * scale.z) + position.z;
		BB.MIN_X = (BB.MIN_X * scale.x) + position.x;
		BB.MIN_Y = (BB.MIN_Y * scale.y) + position.y;
		BB.MIN_Z = (BB.MIN_Z * scale.z) + position.z;
		return mMeshes.at(index).MinMaxPoints;
	}*/

} // namespace Epsilon