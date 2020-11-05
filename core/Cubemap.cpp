#include <CubeMap.h>
namespace Epsilon {

CubeMap::CubeMap(int ID, glm::vec3 Pos)
		{

			mPosition = Pos;
			this->ID = ID;
			type = DYNAMIC;
			prefilterShader = (std::shared_ptr<Shader>)new Shader("shaders/prefilter.vglsl", "shaders/prefilter.glsl");
			mMainShader = (std::shared_ptr<Shader>)new Shader("shaders/vertex.glsl", "shaders/fragment.frag");
			resolution = 16;
			genFrameBuffer();

			captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 1000.0f);

			captureViews[0] = glm::lookAt(mPosition, glm::vec3(1.0f, 0.0f, 0.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[1] = glm::lookAt(mPosition, glm::vec3(-1.0f, 0.0f, 0.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[2] = glm::lookAt(mPosition, glm::vec3(0.0f, 1.0f, 0.0f) + mPosition, glm::vec3(0.0f, 0.0f, 1.0f));
			captureViews[3] = glm::lookAt(mPosition, glm::vec3(0.0f, -1.0f, 0.0f) + mPosition, glm::vec3(0.0f, 0.0f, -1.0f));
			captureViews[4] = glm::lookAt(mPosition, glm::vec3(0.0f, 0.0f, 1.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));
			captureViews[5] = glm::lookAt(mPosition, glm::vec3(0.0f, 0.0f, -1.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));
		}

        CubeMap::CubeMap(std::vector<std::string> paths, int ID, glm::vec3 Pos)
		{
			texture = std::move((std::shared_ptr<eTexture>)(new eTexture(paths)));
			std::cout << "CubeMapID: " << texture->getTextureID() << std::endl;

			mPosition = Pos;
			this->ID = ID;
			type = STATIC;
		}

        void CubeMap::Begin(int index)
		{

			glClearColor(1.0, 0.05, 0.1, 1.0);
			glViewport(0, 0, resolution, resolution);
			glEnable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

			mMainShader->Use();
			mMainShader->PushUniform("projection", captureProjection);


			mMainShader->PushUniform("view", captureViews[index]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, mTexture, 0);

			glGenerateMipmap(GL_TEXTURE_2D);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
        void CubeMap::pushViewMatrix(glm::mat4 view)
		{
			mMainShader->Use();
			mMainShader->PushUniform("view", view);
		}

        void CubeMap::End()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
        void CubeMap::genFrameBuffer()
		{

			glGenFramebuffers(1, &captureFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
			glGenTextures(1, &mTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
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
        void CubeMap::genAmbientConvolution()
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
			glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
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
					glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
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
			mTexture = prefilterMap;
		}
}