#pragma once

#include "TimeSteppingAdaptor.h"

namespace godzilla {

/// Basic time stepping adaptivity
///
class BasicTSAdapt : public TimeSteppingAdaptor {
public:
    BasicTSAdapt(const InputParameters & params);

protected:
    virtual void set_type();

public:
    static InputParameters valid_params();
};

} // namespace godzilla
