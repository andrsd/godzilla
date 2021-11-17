#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

class Problem;

/// Base class for doing output
///
/// Inherit from this class and override `output()` where you can implement your
/// own output code
class Output : public Object, public PrintInterface {
public:
    Output(const InputParameters & params);

    /// Get the file name with the output file produced by this outputter
    ///
    /// @return The file name with the output
    virtual const std::string & getFileName() const = 0;
    /// Set the file name for single output
    virtual void setFileName() = 0;
    /// Set the file name for a sequence of outputs
    ///
    /// @param stepi Step number
    virtual void setSequenceFileName(unsigned int stepi) = 0;
    /// Implement this method to do the desired output
    virtual void output(DM dm, Vec vec) const = 0;

protected:
    /// Problem to get data from
    const Problem & problem;

public:
    static InputParameters validParams();
};

} // namespace godzilla
