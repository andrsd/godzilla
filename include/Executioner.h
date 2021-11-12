#pragma once

#include <vector>
#include "Object.h"
#include "PrintInterface.h"

namespace godzilla {

class Problem;
class Output;

/// Executioner
///
class Executioner : public Object, public PrintInterface {
public:
    Executioner(const InputParameters & parameters);

    virtual void create();
    virtual void execute();

    /// Add and output object
    ///
    /// @param output Output object to add
    virtual void addOutput(const Output * output);

    /// Execute all output object so they can produce their outputs
    virtual void output();

protected:
    Problem & problem;
    /// List of output objects
    std::vector<const Output *> outputs;

public:
    static InputParameters validParams();
};

} // namespace godzilla
