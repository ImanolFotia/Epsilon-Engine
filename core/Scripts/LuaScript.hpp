#include <iostream>
#include <string>
#include <vector>

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

namespace Scripts {
class LuaScript {
    public:
        LuaScript(){
            L = luaL_newstate();
            luaopen_base(L);             /* opens the basic library */
            luaopen_table(L);            /* opens the table library */
            luaopen_io(L);               /* opens the I/O library */
            luaopen_string(L);           /* opens the string lib. */
            luaopen_math(L);
            luaL_requiref(L, "io", luaopen_io, 1);
            luaL_requiref(L, "os", luaopen_os, 1);
        }
        virtual ~LuaScript() {
            if(L) lua_close(L);
        }

        void Load(const char* filename) {
            if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
                std::cout<<"Error: script not loaded ("<<filename<<")"<<std::endl;
                L = 0;
            }
        }
        void printError(const std::string& variableName, const std::string& reason) {
            std::cout<<"Error: can't get ["<<variableName<<"]. "<<reason<<std::endl;
        }

        // Generic get
        template<typename T>
        T lua_get(const std::string& variable_name) {
            return 0;
        }

        // Generic get
        template<typename T>
        T lua_get() {
            return 0;
        }

        template<typename T>
        inline T lua_getdefault() {
            return 0;
        }

        template<typename T>
        T Get(const std::string& variableName) {
            if(!L) {
                printError(variableName, "Script is not loaded");
                return lua_getdefault<T>();
            }

            T result;
            if(lua_gettostack(variableName)) { // variable succesfully on top of stack
                result = lua_get<T>(variableName);
            } else {
                result = lua_getdefault<T>();
            }

            int level = lua_gettop(L);
            lua_pop(L, level + 1);// pop all existing elements from stack
            return result;
        }

        bool lua_gettostack(const std::string& variableName) {
            int level = 0;
            std::string var = "";
            for(unsigned int i = 0; i < variableName.size(); i++) {
                if(variableName.at(i) == '.') {
                    if(level == 0) {
                        lua_getglobal(L, var.c_str());
                    } else {
                        lua_getfield(L, -1, var.c_str());
                    }

                    if(lua_isnil(L, -1)) {
                        printError(variableName, var + " is not defined");
                        return false;
                    } else {
                        var = "";
                        level++;
                    }
                } else {
                    var += variableName.at(i);
                }
            }
            if(level == 0) {
                lua_getglobal(L, var.c_str());
            } else {
                lua_getfield(L, -1, var.c_str());
            }
            if(lua_isnil(L, -1)) {
                printError(variableName, var + " is not defined");
                return false;
            }

            return true;
        }

        void clean() {

        }

        template <typename T>
        std::vector<T> getTable(std::string tableName) {
            std::vector<T> v;
            lua_getglobal(L, tableName.c_str());
            if(lua_isnil(L, -1)) {
                return std::vector<T>();
            }
            lua_pushnil(L);
            while(lua_next(L, -2)) {
                v.push_back((T)lua_get<T>());
                lua_pop(L, 1);
            }
            clean();
            return v;
        }

        template<typename T, class ...Args>
        T Exec(std::string func_name, Args... args) {
            lua_getglobal(L, func_name.c_str());
            int numArgs = 0;
            for(auto arg: {args...})
            {
                numArgs++;
                push_arg(arg);
            }
            lua_pcall(L, numArgs, 1, 0);
            T result = (int)lua_get<T>();
            lua_pop(L, 1);
            return result;
        }
    private:

        template <typename T>
        void push_arg(T arg) {
            return;
        }

        lua_State* L;
    protected:
};

template<>
inline std::string LuaScript::lua_getdefault() {
  return "null";
}

template<>
inline int LuaScript::lua_get(const std::string& variable_name) {
    if(!lua_isnumber(L, -1)) {
        printError(variable_name, "Not a number");
    }
    return (int)lua_tonumber(L, -1);
}
template<>
inline float LuaScript::lua_get(const std::string& variable_name) {
    if(!lua_isnumber(L, -1)) {
        printError(variable_name, "Not a number");
    }
    return (float)lua_tonumber(L, -1);
}
template<>
inline std::string LuaScript::lua_get(const std::string& variable_name) {
    std::string s = "null";
    if(lua_isstring(L, -1)) {
        s = std::string(lua_tostring(L, -1));
    } else {
        printError(variable_name, "Not a string");
    }
    return s;
}
template<>
inline bool LuaScript::lua_get(const std::string& variable_name) {
    return (bool)lua_toboolean(L, -1);
}


template<>
inline int LuaScript::lua_get() {
    if(!lua_isnumber(L, -1)) {
        printError("", "Not a number");
    }
    return (int)lua_tonumber(L, -1);
}
template<>
inline float LuaScript::lua_get() {
    if(!lua_isnumber(L, -1)) {
        printError("", "Not a number");
    }
    return (float)lua_tonumber(L, -1);
}
template<>
inline std::string LuaScript::lua_get() {
    std::string s = "null";
    if(lua_isstring(L, -1)) {
        s = std::string(lua_tostring(L, -1));
    } else {
        printError("", "Not a string");
    }
    return s;
}
template<>
inline bool LuaScript::lua_get() {
    return (bool)lua_toboolean(L, -1);
}


template <>
void LuaScript::push_arg(int arg) {
    lua_pushnumber(L, arg);
}

template <>
void LuaScript::push_arg(std::string arg) {
    lua_pushstring(L, arg.c_str());
}
}
