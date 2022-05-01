#pragma once

#include "Object.h"
#include "PrintInterface.h"

namespace godzilla {

class Problem;
class Mesh;

/// Base class for doing output
///
class Output : public Object, public PrintInterface {
public:
    Output(const InputParameters & params);

    /// Do the desired output
    virtual void output() = 0;

protected:
    /// Problem to get data from
    const Problem * problem;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
