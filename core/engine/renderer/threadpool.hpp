#pragma once

#include <thread>
#include <vector>
#include <functional>

namespace engine {
	
	class Task {

		enum class TaskState {
			Done = 0,
			Working
		};

		std::function<void<void>> m_pFunction;
		TaskState m_pState;

	public: 
		Task() = default;

		TaskState State() {
			return m_pState;
		}

		void Launch() {
			m_pFunction();
		}

		friend class ThreadPool;
	};
	
	class ThreadPool {
		std::vector<std::thread> m_pThreads;

	public:
		ThreadPool() {
			m_pThreads.resize(std::thread::hardware_concurrency);
		}

		ThreadPool(size_t thread_count) {
			m_pThreads.resize(thread_count);
		}


	};
}