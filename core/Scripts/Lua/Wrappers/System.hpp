#pragma once

#include <IO/KeyBoard.h>
#include <IO/Mouse.h>

namespace Epsilon::Lua::System::Keyboard {

    const char* lib_name = "epsilon.system.keyboard";

    int getKey(lua_State *L)
    {
        luaL_argcheck(L, lua_isnumber(L, 2), 2, "not a number");
        int a = (uint16_t)lua_tonumber(L, 2);
        bool state = Input::KeyBoard::getKey(a)
        lua_pushboolean(L, state);

        return 1;
    }

    int pretty_call(lua_State *L)
    {
        std::string memberName = lua_tostring(L, 2);

        int result = 1;
        if (memberName == "getKey")
        {
            lua_pushcfunction(L, getKey);
        }
        else
        {
            result = 0;
        }

        return result;
    }

    const struct luaL_Reg keyboard_lib[] = {
        {"getKey", getKey},
        {nullptr, nullptr} // sentinel
    };

    const struct luaL_Reg keyboard_meta[] = {
        {"__index", pretty_call},
        {nullptr, nullptr} // sentinel
    };

    int load(lua_State *L)
    {
        int res = luaL_newmetatable(L, lib_name);
        std::cout << (res == 0 ? "already in use" : "") << std::endl;

        luaL_setfuncs(L, keyboard_meta, 0);
        luaL_newlib(L, keyboard_lib);

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