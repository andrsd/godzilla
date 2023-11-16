#pragma once

#include "godzilla/TimeSteppingAdaptor.h"

namespace godzilla {

/// Basic time stepping adaptivity
///
class BasicTSAdapt : public TimeSteppingAdaptor {
public:
    explicit BasicTSAdapt(const Parameters & params);

protected:
    void set_type() override;

public:
    static Parameters parameters();
};

} // namespace godzilla
