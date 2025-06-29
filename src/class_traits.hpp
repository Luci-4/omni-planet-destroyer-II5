#pragma once


class NonMovable 
{
public:
    NonMovable() = default;
    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;
};

class NonCopyable
{
public:
    NonCopyable() = default;
    NonCopyable(NonCopyable const&) = delete;
    NonCopyable& operator=(NonCopyable const&) = delete;
};

class NonMovableNonCopyable  : public NonCopyable, public NonMovable {};

