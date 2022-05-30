#pragma once

#include "Object.h"
#include "PrintInterface.h"
#include "petscdm.h"

namespace godzilla {

class Problem;

/// Base class for doing output
///
class Output : public Object, public PrintInterface {
public:
    Output(const InputParameters & params);

    virtual void create() override;

    /// Set execute mask
    ///
    /// @param mask Bit mask for execution
    virtual void set_exec_mask(unsigned int mask);

    /// Should output happen at a specified occasion
    ///
    /// @param mask Bit mask specifying the occasion, see ON_XYZ below
    /// @return `true` if output should happen, otherwise `false`
    virtual bool should_output(unsigned int mask);

    /// Output a step of a simulation
    virtual void output_step() = 0;

protected:
    /// Set up the execution mask
    void set_up_exec();

    /// Problem to get data from
    const Problem * problem;

    /// Bitwise mask for determining when this output object should output its content
    unsigned int on;

public:
    static InputParameters valid_params();

    static const unsigned int ON_NONE = 0x0;
    static const unsigned int ON_INITIAL = 0x1;
    static const unsigned int ON_TIMESTEP = 0x2;
    static const unsigned int ON_FINAL = 0x4;
};

} // namespace godzilla
