#pragma once

#include <Entity/Entity.h>

namespace Epsilon::Lua::Entity
{
    const char *lib_name = "epsilon.entity";
    std::vector<std::shared_ptr<EntityBase>> *localEntityList;

    int addEntity(lua_State *L)
    {
        luaL_argcheck(L, lua_isnumber(L, 2), 2, "not a number");
        float x = lua_tonumber(L, 2);

        luaL_argcheck(L, lua_isnumber(L, 2), 2, "not a number");
        float y = lua_tonumber(L, 3);

        luaL_argcheck(L, lua_isnumber(L, 2), 2, "not a number");
        float z = lua_tonumber(L, 4);

        glm::vec3 tPosition = glm::vec3(x, y, z);
        glm::vec3 tScale = glm::vec3(1.0f);
        glm::quat tRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);
        _Entity->setName(std::string("Entity") + std::to_string(localEntityList->size()));
        localEntityList->push_back(_Entity);

        lua_pushnumber(L, localEntityList->size() - 1);

        return 1;
    }

    int deleteEntity(lua_State *L)
    {
        return 0;
    }

    int addComponent(lua_State *L)
    {
        return 0;
    }

    int removeComponent(lua_State *L)
    {
        return 0;
    }

    const struct luaL_Reg entity_lib[] = {
        {"addEntity", addEntity},
        {"deleteEntity", deleteEntity},
        {"addComponent", addComponent},
        {"removeComponent", removeComponent},
        {nullptr, nullptr} // sentinel
    };

    const struct luaL_Reg entity_meta[] = {
        {nullptr, nullptr} // sentinel
    };

    int load(lua_State *L)
    {
        int res = luaL_newmetatable(L, lib_name);
        std::cout << (res == 0 ? "already in use" : "") << std::endl;

        luaL_setfuncs(L, entity_meta, 0);
        luaL_newlib(L, entity_lib);

        luaL_getmetatable(L, lib_name);
        lua_setmetatable(L, -2);

        return 1;
    }

    void Export(lua_State *L)
    {
        luaL_requiref(L, lib_name, load, 1);
        lua_setglobal(L, lib_name);
    }
}