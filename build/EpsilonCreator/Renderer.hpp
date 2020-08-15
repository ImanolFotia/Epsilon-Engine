#pragma once

namespace EpsilonCreator {
	class Renderer {
	public:
		Renderer();

		[[maybe_unused]] void OnUpdate();

		[[maybe_unused]] void OnRender();

		[[maybe_unused]] void Flush();

		[[maybe_unused]] void OnDestroy();

	public:

		void AddEntity();

		void RemoveEntity();

	private:
		int m_numEntities = 0;
	};
}