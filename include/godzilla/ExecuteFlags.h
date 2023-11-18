#pragma once

namespace godzilla {

class ExecuteOn {
public:
    enum ExecuteOnFlag : unsigned int { INITIAL = 0x1, TIMESTEP = 0x2, FINAL = 0x4 };

    ExecuteOn() : mask(0) {}

    ExecuteOn(ExecuteOnFlag flag) : mask(flag) {}

    bool
    has_flags() const
    {
        return this->mask != 0;
    }

    ExecuteOn &
    operator|=(ExecuteOnFlag rhs)
    {
        this->mask |= rhs;
        return *this;
    }

    bool
    operator&(ExecuteOnFlag flag)
    {
        return (this->mask & flag);
    }

private:
    unsigned int mask;
};

inline ExecuteOn
operator|(ExecuteOn::ExecuteOnFlag one, ExecuteOn::ExecuteOnFlag two)
{
    ExecuteOn flags(one);
    flags |= two;
    return flags;
}

} // namespace godzilla
