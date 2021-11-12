#pragma once

#include "Object.h"
#include "PrintInterface.h"

namespace godzilla {

class Problem;

/// Base class for doing output
///
/// Inherit from this class and override `output()` where you can implement your
/// own output code
class Output : public Object, public PrintInterface {
public:
    Output(const InputParameters & params);

    /// Implement this method to do the desired output
    virtual void output() const = 0;

protected:
    /// Problem to get data from
    const Problem & problem;

public:
    static InputParameters validParams();
};

} // namespace godzilla
