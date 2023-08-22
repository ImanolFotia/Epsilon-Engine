#pragma once

namespace engine {
	class ComputeDispatch {
	public:
		ComputeDispatch() = default;

		virtual void Dispatch() = 0;
	};
}