#pragma once

#include "TimeSteppingAdaptor.h"

namespace godzilla {

/// Basic time stepping adaptivity
///
class BasicTSAdapt : public TimeSteppingAdaptor {
public:
    BasicTSAdapt(const Parameters & params);

protected:
    virtual void set_type();

public:
    static Parameters valid_params();
};

} // namespace godzilla
