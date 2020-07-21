#pragma once

#include "Component.h"

#include <Scripts/LuaScript.hpp>

namespace Epsilon {

    namespace Component {

    class ScriptComponent : public Component {
        public:
            ScriptComponent(const char* scriptPath) { 
                mType = COMPONENT_TYPE::SCRIPTCOMPONENT;
                mLuaScript = std::make_shared<Script::Lua>();
                mLuaScript->Load(scriptPath);
                mLuaScript->Exec<int>("OnInit");
            }

            ~ScriptComponent() {
                mLuaScript->Exec<int>("OnTerminate");
            }

            void setParent(EntityBase* parent) {
                mParent = parent;
            }

            void Update(){
                mLuaScript->Exec<int>("OnUpdate");
            }

            void Render(){}

            glm::vec3 getPosition() { return glm::vec3(0.0); }
            glm::quat getRotation(){ return glm::quat(0.0, 0.0, 0.0, 0.0); }
            glm::vec3 getScale(){ return glm::vec3(0.0); }

            COMPONENT_TYPE getType(){
                return mType;
            }

            private:
                std::shared_ptr<Script::Lua> mLuaScript;
                COMPONENT_TYPE mType;
    };
    }
}