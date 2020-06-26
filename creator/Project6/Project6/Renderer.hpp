#pragma once

#include <Renderer/Renderer.hpp>

#include <vector>

namespace EpsilonCreator
{
	class Renderer {
	public:
		Renderer() = default;

		void Init() {}

		void Update() {}

		void Render() {}

	private:
		std::vector<Epsilon::Model> mvModels;
	};

}