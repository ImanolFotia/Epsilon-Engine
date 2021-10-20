#include <Core.hpp>
#include <Physics/Physics.h>
#include <Physics/TriangleMeshPhysicObject.h>
#include <Resource/ResourceManager.h>
#include <BSPFace.h>

namespace Epsilon
{
	bool BSPFace::BuildFace(std::vector<glm::vec3> Vertices,
							std::vector<glm::vec3> Normals,
							std::vector<glm::vec2> TexCoords,
							std::vector<glm::vec2> LMTexCoords,
							std::vector<unsigned int> Indices,
							int ID, string imagePath,
							tBSPLightmap LightMap)
	{ 
		this->Vertices = Vertices;
		this->TexCoords = TexCoords;
		this->Indices = Indices; 
		this->Normals = Normals;
		this->faceID = ID;
		this->ObjectID = "BSPFace_" + Helpers::intTostring(ID);
		this->imagePath = imagePath;
		this->imagePath = this->imagePath;
		this->LightMap = LightMap;
		this->LMTexCoords = LMTexCoords;

		this->CalcTangentSpace();
		for (unsigned int i = 0; i < Vertices.size(); i++)
		{
			t_Vertex vert;
			vert.position = Vertices[i];
			vert.texcoord = TexCoords[i];
			vert.normal = glm::normalize(-Normals[i]);
			vert.tangent = -Tangents[i];
			vert.bitangent = -Bitangents[i];
			mVertices.push_back(vert);

			mPosition *= Vertices[i] * (1.0f / Vertices.size());
		}
 
		mPosition /= Vertices.size();
		this->prepareVAO();
		std::shared_ptr<Physics::TriangleMeshPhysicObject> ph = (std::shared_ptr<Physics::TriangleMeshPhysicObject>)new Physics::TriangleMeshPhysicObject();
		rigidBody = nullptr;
		rigidBody = ph->addObject(this->Vertices, this->Indices, 0.1);
		collinfo->setName(this->ObjectID);
		ph->Body->setUserPointer(collinfo.get());
		ResourceManager::Get().getPhysicsWorld()->getSoftDynamicsWorld()->addRigidBody(rigidBody.get());
		this->CollisionObject = ph;
		//this->LoadLightMapTexture();
		return true;
	}

	void BSPFace::setMaterial(Shader_ptr shader) {

		auto & Instance = ResourceManager::Get();

		Instance.getTexture2D(albedoTexture)->Bind(GL_TEXTURE0);
		shader->PushUniform("texture_diffuse", 0);
        shader->PushUniform("using_color_diffuse", false);
		

		Instance.getTexture2D(roughnessTexture)->Bind(GL_TEXTURE1);
		shader->PushUniform("texture_specular", 1);
        shader->PushUniform("using_color_specular", false);

		Instance.getTexture2D(normalTexture)->Bind(GL_TEXTURE2);
		shader->PushUniform("texture_normal", 2);
        shader->PushUniform("using_color_normal", false);

		Instance.getTexture2D(metallicTexture)->Bind(GL_TEXTURE3);
		shader->PushUniform("texture_height", 3);
        shader->PushUniform("using_color_height", false);
	}

	void BSPFace::RenderFace(Shader_ptr shader, bool simpleRender, bool change_material)
	{

		auto & Instance = ResourceManager::Get();

		if(Instance.cubemapsLoaded && this->CubemapId == -1)
		{
			CubemapId = Instance.NearestCubeMap(mPosition);
			GIProbeID = Instance.NearestCubeMap(mPosition) - 1;
		}

		if(change_material) setMaterial(shader);

		glActiveTexture(GL_TEXTURE4);
		shader->PushUniform("skybox", 4);
		if(this->CubemapId != -1)
		glBindTexture(GL_TEXTURE_CUBE_MAP, Instance.useCubeMap(CubemapId));
  
		mVAO->Bind();  
		glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
		mVAO->Unbind();
	}        
} // namespace Epsilon  