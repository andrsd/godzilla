#pragma once

#include <vector>
#include "base/Object.h"
#include "base/GPrintInterface.h"


namespace godzilla {

class GProblem;
class GOutput;

/// Executioner
///
class GExecutioner : public Object,
                     public GPrintInterface
{
public:
    GExecutioner(const InputParameters & parameters);

    virtual void create();
    virtual void execute();

    /// Add and output object
    ///
    /// @param output Output object to add
    virtual void addOutput(const GOutput * output);

    /// Execute all output object so they can produce their outputs
    virtual void output();

protected:
    GProblem & problem;
    /// List of output objects
    std::vector<const GOutput *> outputs;

public:
    static InputParameters validParams();
};

}
