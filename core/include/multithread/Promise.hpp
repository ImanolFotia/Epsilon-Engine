#pragma once
#include <Lambda>
#include <string>
#include <exception>

namespace Threading{
    class Promise{
    public:
        Promise(){}

        Promise(Lambda l) : _primary_function(l){
            try
            {

            }
            catch(std::exception e)
            {
                _catch_function(e.what());
            }

            _then_function();
        }

        void then(Lambda<void(Lambda success, Lambda faillure)> l) : _then_function(l){
            //_then_function();
        }

        void catch(Lambda <void(std::string)> l) : _catch_function(l){
            //_catch_function();
        }
    private:

        Lambda _primary_function;
        Lambda<void(Lambda, Lambda)> _then_function;
        Lambda<void(std::string)> _catch_function;

    };
}

