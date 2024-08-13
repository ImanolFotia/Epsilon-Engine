//
// Created by solaire on 2/3/23.
//

#ifndef EPSILON_SINGLETON_HPP
#define EPSILON_SINGLETON_HPP

template <typename T>
class singleton
{
public:
    static T self;

public:
    static T &getSingleton()
    {
        return self;
    }
};

#endif // EPSILON_SINGLETON_HPP
