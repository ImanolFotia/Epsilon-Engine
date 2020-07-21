#pragma once
namespace Epsilon
{
    class Registry;
    class Entity
    {
    public:
        Entity() = default;

        uint32_t Id() { return mId; }

        uint32_t Parent() { return mParent; }

        friend Registry;

    private:
        void Id(uint32_t id)
        {
            mId = id;
        }

        uint32_t mId;
        uint32_t mParent;
    };
} // namespace Epsilon