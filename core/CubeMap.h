#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

#include <Texture.h>
#include <Driver/API/OpenGL/HelperFunctions/FullScreenQuad.h>
#include <Driver/API/OpenGL/FrameBuffer.h>
#include <Shader.h>
#include <Types.h>
#include <cubemapRenderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <exception>

namespace Epsilon
{
	class CubeMap
	{
	public:
		CubeMap(int ID, glm::vec3 Pos)
		{

			Position = Pos;
			this->ID = ID;
			type = DYNAMIC;
			prefilterShader = (std::shared_ptr<Shader>)new Shader("shaders/prefilter.vglsl", "shaders/prefilter.glsl");
			mMainShader = (std::shared_ptr<Shader>)new Shader("shaders/vertex.glsl", "shaders/fragment.frag");
			resolution = 32;
			genFrameBuffer();

			captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 1000.0f);

			captureViews[0] = glm::lookAt(Position, glm::vec3(1.0f, 0.0f, 0.0f) + Position, glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[1] = glm::lookAt(Position, glm::vec3(-1.0f, 0.0f, 0.0f) + Position, glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[2] = glm::lookAt(Position, glm::vec3(0.0f, 1.0f, 0.0f) + Position, glm::vec3(0.0f, 0.0f, 1.0f));
			captureViews[3] = glm::lookAt(Position, glm::vec3(0.0f, -1.0f, 0.0f) + Position, glm::vec3(0.0f, 0.0f, -1.0f));
			captureViews[4] = glm::lookAt(Position, glm::vec3(0.0f, 0.0f, 1.0f) + Position, glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[5] = glm::lookAt(Position, glm::vec3(0.0f, 0.0f, -1.0f) + Position, glm::vec3(0.0f, -1.0f, 0.0f));
		}

		CubeMap(std::vector<std::string> paths, int ID, glm::vec3 Pos)
		{
			texture = std::move((std::shared_ptr<eTexture>)(new eTexture(paths)));
			std::cout << "CubeMapID: " << texture->getTextureID() << std::endl;

			Position = Pos;
			this->ID = ID;
			type = STATIC;
		}

		virtual ~CubeMap()
		{
			/*std::cout << "Deleted CubeMap" << std::endl;*/
		}

		virtual void CaptureEnvironment(int index)
		{

			//glClearColor(0.05,0.08,0.2, 1.0);
			glClearColor(1.0, 0.05, 0.1, 1.0);
			glViewport(0, 0, resolution, resolution);
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

			mMainShader->Use();
			mMainShader->PushUniform("projection", captureProjection);

			//glUniformMatrix4fv(mMainShader->getUniformLocation("view"), 1, GL_FALSE, (const float*)&captureViews[index]);

			mMainShader->PushUniform("view", captureViews[index]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, cubemapTex, 0);

			glGenerateMipmap(GL_TEXTURE_2D);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void pushViewMatrix(glm::mat4 view)
		{
			mMainShader->Use();
			//glUniformMatrix4fv(mMainShader->getUniformLocation("view"), 1, GL_FALSE, (const float*)&view);
			mMainShader->PushUniform("view", view);
		}

		void endCapturingEnvironment()
		{
			//renderer->End();
			//this->mMainShader->Free();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void genFrameBuffer()
		{

			glGenFramebuffers(1, &captureFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glGenTextures(1, &cubemapTex);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
			//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
			//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, 0);
			}

			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			glGenRenderbuffers(1, &captureRBO);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, resolution, resolution);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
		}

		void genAmbientConvolution()
		{
			glGenTextures(1, &prefilterMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			for (unsigned int i = 0; i < 6; ++i)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			captureViews[0] = glm::lookAt(glm::vec3(0.0), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[1] = glm::lookAt(glm::vec3(0.0), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[2] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			captureViews[3] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			captureViews[4] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[5] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

			// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
			// ----------------------------------------------------------------------------------------------------
			prefilterShader->Use();
			prefilterShader->PushUniform("projection", captureProjection);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
			prefilterShader->PushUniform("environmentMap", 0);

			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				// reisze framebuffer according to mip-level size.
				unsigned int mipWidth = resolution * std::pow(0.5, mip);
				unsigned int mipHeight = resolution * std::pow(0.5, mip);
				glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, mipWidth, mipHeight);
				glViewport(0, 0, mipWidth, mipHeight);
				glEnable(GL_DEPTH_TEST);

				float roughness = (float)mip / (float)(maxMipLevels - 1);
				prefilterShader->PushUniform("roughness", roughness);
				for (unsigned int i = 0; i < 6; ++i)
				{
					prefilterShader->Use();
					prefilterShader->PushUniform("projection", captureProjection);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
					prefilterShader->PushUniform("environmentMap", 0);

					//glUniformMatrix4fv(prefilterShader->getUniformLocation("view"), 1, GL_FALSE, (const float*)&captureViews[i]);
					prefilterShader->PushUniform("view", captureViews[i]);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					renderCube();
				}
			}
			prefilterShader->Free();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			cubemapTex = prefilterMap;
		}

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
			return this->Position;
		}

		virtual GLuint getTextureID()
		{
			if (type == STATIC)
				return texture->getTextureID();
			if (type == DYNAMIC)
				return cubemapTex;

			return 0;
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
		GLuint cubemapTex;
		std::shared_ptr<Shader> mMainShader;
		std::shared_ptr<Shader> prefilterShader;
		glm::mat4 captureProjection;

		unsigned int prefilterMap;
		GLuint captureFBO = 0;
		GLuint captureRBO = 0;

	private:
		int ID = 0;
		glm::vec3 Position;
		std::shared_ptr<eTexture> texture;
		std::vector<glm::vec3> m_CubeMapsDirections;
		std::vector<GLuint> m_CubemapSides;
		std::vector<GLuint> m_renderTargets;
		std::shared_ptr<OpenGLHelpers::FullScreenQuad> m_Quad;
		std::shared_ptr<Shader> mPassThroughShader;
		std::shared_ptr<FrameBuffer<std::string>> hdrFBO;
		CUBEMAP_TYPE type;
		int resolution;

	protected:
		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;
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