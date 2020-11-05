#pragma once

namespace Epsilon
{
	namespace Physics
	{
		
        enum Type
        {
            CUBE = 0,
            SPHERE,
            TRIANGLE_MESH,
            CLOTH,

        };
		class CollisionInfo
		{
		public:
			CollisionInfo()
			{
				m_Name = "";
			}
			//~CollisionInfo() {}

			std::string getName()
			{
				return m_Name;
			}

			void setName(std::string name)
			{
				m_Name = name;
			}

			void setType(enum Type t) {
				mType = t;
			}

			enum Type getType() {
				return mType;
			}

		private:
			enum Type mType;
			std::string m_Name;
		};
	} // namespace Physics
} // namespace Epsilon