#include <CubeMap.h>
#include <Engine.hpp>

namespace Epsilon
{

	CubeMap::CubeMap(int ID, glm::vec3 Pos)
	{

		mPosition = Pos;
		this->ID = ID;
		type = DYNAMIC;
		prefilterShader = (std::shared_ptr<Shader>)new Shader("shaders/prefilter.vglsl", "shaders/prefilter.glsl");
		
		resolution = Engine::Get().Settings().AMBIENT_PROBE_RES;
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

		captureViews[0] = glm::lookAt(mPosition, glm::vec3(1.0f, 0.0f, 0.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));
		captureViews[1] = glm::lookAt(mPosition, glm::vec3(-1.0f, 0.0f, 0.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));
		captureViews[2] = glm::lookAt(mPosition, glm::vec3(0.0f, 1.0f, 0.0f) + mPosition, glm::vec3(0.0f, 0.0f, 1.0f));
		captureViews[3] = glm::lookAt(mPosition, glm::vec3(0.0f, -1.0f, 0.0f) + mPosition, glm::vec3(0.0f, 0.0f, -1.0f));
		captureViews[4] = glm::lookAt(mPosition, glm::vec3(0.0f, 0.0f, 1.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));
		captureViews[5] = glm::lookAt(mPosition, glm::vec3(0.0f, 0.0f, -1.0f) + mPosition, glm::vec3(0.0f, -1.0f, 0.0f));

		glClearColor(1.0, 0.05, 0.1, 1.0);
		glViewport(0, 0, resolution, resolution);
		glEnable(GL_DEPTH_TEST);
		
		mFramebuffer->bindFramebuffer();
		mFramebuffer->AttachRenderBuffer(0, index, 0);

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
		mFramebuffer->unbindFramebuffer();
	}
	void CubeMap::genFrameBuffer()
	{
		if (mFramebuffer == nullptr)
		{
			mFramebuffer = std::make_shared<OpenGL::FrameBuffer<int>>(resolution, resolution, true);

			mFramebuffer->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, true, GL_TEXTURE_CUBE_MAP);
		}

		mFramebuffer->addDepthAttachment();
		mFramebuffer->FinishFrameBuffer();
	}

	void CubeMap::genAmbientConvolution()
	{
		isConvoluted = true;

		if (mPrefilter == nullptr) {
			mPrefilter = std::make_shared<Renderer::TextureCube>();
			API::Texture::TextureData data;
			data.MakeDefaultGL();
			data.InternalFormat = GL_RGB16F;
			data.Format = GL_RGB;
			data.Type = GL_FLOAT;
			data.Width = resolution;
			data.Height = resolution;
			data.AnisotropyLevel = 0;
			data.Filtering = 2;
			data.Wrapping = GL_CLAMP_TO_EDGE;
			mPrefilter->Create(data);
		}
		
		mPrefilter->Bind();
		mPrefilter->setData(nullptr, 0);
		mPrefilter->Unbind();

		captureViews[0] = glm::lookAt(glm::vec3(0.0), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		captureViews[1] = glm::lookAt(glm::vec3(0.0), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		captureViews[2] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		captureViews[3] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		captureViews[4] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		captureViews[5] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

		// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
		// ----------------------------------------------------------------------------------------------------
		prefilterShader->Use();
		mFramebuffer->bindFramebuffer();
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = resolution * std::pow(0.5, mip);
			unsigned int mipHeight = resolution * std::pow(0.5, mip);
			mFramebuffer->BindDepthBuffer(mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);
			glEnable(GL_DEPTH_TEST);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader->PushUniform("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader->Use();
				prefilterShader->PushUniform("projection", captureProjection);
				glActiveTexture(GL_TEXTURE0);
				auto texture_handler = mFramebuffer->getRenderTargetHandler(0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture_handler);
				prefilterShader->PushUniform("environmentMap", 0);

				prefilterShader->PushUniform("view", captureViews[i]);
				
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mPrefilter->ID(), mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				renderCube();
			}
		}
		prefilterShader->Free();
		
		mFramebuffer->unbindFramebuffer();
		
		mFramebuffer->BindDepthBuffer(resolution, resolution);
	}

	void CubeMap::Destroy()
	{
		if (mTexture != 0)
		{
			glDeleteRenderbuffers(1, &captureRBO);
			glDeleteFramebuffers(1, &captureFBO);
			glDeleteTextures(1, &mTexture);
			glDeleteTextures(1, &prefilterMap);
		}
	}
}