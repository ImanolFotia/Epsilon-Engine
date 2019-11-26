#pragma once
namespace Physics {
	class CollisionInfo {
		public:
			CollisionInfo() {
				m_Name = "";
			}
			~CollisionInfo() {}

			std::string getName() {
				return m_Name;
			}

			void setName(std::string name) {
				m_Name = name;
			}

		private:
			std::string m_Name;
	};
}
