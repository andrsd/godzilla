#pragma once

#include "godzilla/Object.h"
#include "godzilla/PrintInterface.h"
#include "godzilla/Types.h"

namespace godzilla {

class Problem;

/// Base class for doing output
///
class Output : public Object, public PrintInterface {
public:
    explicit Output(const Parameters & params);

    void create() override;
    void check() override;

    /// Set execute mask
    ///
    /// @param mask Bit mask for execution
    void set_exec_mask(unsigned int mask);

    /// Get execution mask
    ///
    /// @return execution mask
    unsigned int get_exec_mask() const;

    /// Should output happen at a specified occasion
    ///
    /// @param mask Bit mask specifying the occasion, see ON_XYZ below
    /// @return `true` if output should happen, otherwise `false`
    virtual bool should_output(unsigned int mask);

    /// Output a step of a simulation
    virtual void output_step() = 0;

protected:
    /// Get problem
    ///
    /// @return Problem this output is part of
    Problem * get_problem() const;

    /// Set up the execution mask
    void set_up_exec();

private:
    /// Problem to get data from
    Problem * problem;

    /// Bitwise mask for determining when this output object should output its content
    unsigned int on_mask;

    ///
    Int interval;

public:
    static Parameters parameters();

    static const unsigned int ON_NONE;
    static const unsigned int ON_INITIAL;
    static const unsigned int ON_TIMESTEP;
    static const unsigned int ON_FINAL;
};

} // namespace godzilla
