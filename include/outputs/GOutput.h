#pragma once

#include "base/Object.h"
#include "base/GPrintInterface.h"

namespace godzilla {

class GProblem;

/// Base class for doing output
///
/// Inherit from this class and override `output()` where you can implement your
/// own output code
class GOutput : public Object,
                public GPrintInterface
{
public:
    GOutput(const InputParameters & params);

    /// Implement this method to do the desired output
    virtual void output() const = 0;

protected:
    /// GProblem to get data from
    const GProblem & problem;

public:
    static InputParameters validParams();
};

} // godzilla
