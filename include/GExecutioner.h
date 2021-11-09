#pragma once

#include <vector>
#include "Object.h"
#include "GPrintInterface.h"


namespace godzilla {

class Problem;
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
    Problem & problem;
    /// List of output objects
    std::vector<const GOutput *> outputs;

public:
    static InputParameters validParams();
};

}
