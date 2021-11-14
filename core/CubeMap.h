#pragma once

#include <pch.hpp>

#include <Texture.h>
#include <Driver/API/OpenGL/HelperFunctions/FullScreenQuad.h>
#include <Driver/API/OpenGL/FrameBuffer.h>
#include <Shader.h>
#include <Types.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GIProbe.hpp>

#include <Driver/API/Framebuffer.hpp>

#include <Renderer/TextureCube.hpp>

namespace Epsilon
{
	class CubeMap : public GIProbe
	{
	public:
		CubeMap(int, glm::vec3);

		CubeMap(std::vector<std::string> paths, int ID, glm::vec3 Pos);

		virtual ~CubeMap()
		{
			/*std::cout << "Deleted CubeMap" << std::endl;*/
		}

		void pushViewMatrix(glm::mat4 view);
		
		void Begin(int index) override;

		void End() override;

		void genFrameBuffer();

		void genAmbientConvolution();

		void Destroy();

		glm::mat4 getViewMatrixbyIndex(int index)
		{
			return captureViews[index];
		}

		std::shared_ptr<Shader> getShader()
		{
			return this->mMainShader;
		}

		glm::mat4 getProjectionMatrix()
		{
			return this->captureProjection;
		}

		virtual glm::vec3 getPosition()
		{
			return this->mPosition;
		}

		virtual GLuint getTextureID()
		{
			/*if(isConvoluted) {
				if (type == STATIC)
					return texture->getTextureID();
				if (type == DYNAMIC)
					return mTexture;
			} else {
				return mTexture;
			}*/
			if(isConvoluted) return prefilterMap;
			return mTexture;
		}

		std::shared_ptr<Renderer::TextureCube> getTextureCube() {
			if(isConvoluted) return mPrefilter;
			return nullptr;
		}

		virtual GLuint getCubemapFace(int index)
		{
			return m_CubemapSides.at(index);
		}

		int getID()
		{
			return this->ID;
		}

		CUBEMAP_TYPE getType()
		{
			return type;
		}

		glm::mat4 captureViews[6];
		std::shared_ptr<Shader> mMainShader;
		std::shared_ptr<Shader> prefilterShader;
		glm::mat4 captureProjection;

		GLuint prefilterMap = 0;
		GLuint captureFBO = 0;
		GLuint captureRBO = 0;

		std::shared_ptr<OpenGL::FrameBuffer<int>> mFramebuffer;
		std::shared_ptr<Renderer::TextureCube> mPrefilter = nullptr;

	private:
		int ID = 0;
		std::shared_ptr<eTexture> texture;
		std::vector<glm::vec3> m_CubeMapsDirections;
		std::vector<GLuint> m_CubemapSides;
		std::vector<GLuint> m_renderTargets;
		std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_Quad;
		std::shared_ptr<Shader> mPassThroughShader;
		CUBEMAP_TYPE type;
		int resolution;

	protected:
		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;
		bool isConvoluted = false;
		void renderCube()
		{
			// initialize (if necessary)
			if (cubeVAO == 0)
			{
				float vertices[] = {
					// back face
					-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
					1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
					1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// bottom-right
					1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
					-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
					-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	// top-left
					// front face
					-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
					1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
					1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
					1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
					-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
					-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
					// left face
					-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
					-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-left
					-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
					-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
					-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
					-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
					// right face
					1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	  // top-left
					1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
					1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
					1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
					1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	  // top-left
					1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
					// bottom face
					-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
					1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top-left
					1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
					1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
					-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
					-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
					// top face
					-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
					1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
					1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
					1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
					-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
					-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f	  // bottom-left
				};
				glGenVertexArrays(1, &cubeVAO);
				glGenBuffers(1, &cubeVBO);
				// fill buffer
				glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				// link vertex attributes
				glBindVertexArray(cubeVAO);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}
			// render Cube
			glBindVertexArray(cubeVAO);
			glCache::glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	};

	class PCCB : public CubeMap
	{

	public:
		PCCB();

		~PCCB() {}

		std::vector<glm::vec3> getBoxMinMaxPoints()
		{
			return BoxMinMaxPoints;
		}

		std::vector<glm::vec3> BoxMinMaxPoints;
	};
} // namespace Epsilon