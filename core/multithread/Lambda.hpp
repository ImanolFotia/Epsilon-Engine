#pragma once
#include <functional>

namespace Threading{
    template <typename Return, Argument>
    class Lambda{
    public:
        Lambda(std::function<Return(Argument)>){}

        _return_type Exec(){
            _function();
        }

    private:
        Return _return_type;
        Argument _argument_type;

        std::function<_return_type(_argument_type)> _function;
    };
}
