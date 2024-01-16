// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petscts.h"
#include "godzilla/Types.h"
#include "godzilla/Vector.h"

namespace godzilla {

class TimeStepAdapt {
public:
    enum Type { NONE, BASIC, DSP, CFL, GLEE, HISTORY };

    struct Candidate {
        Int order;
        Int stage_order;
        Real ccfl;
        Real cost;
    };

    TimeStepAdapt();
    explicit TimeStepAdapt(TSAdapt tsadapt);

    /// Add a candidate scheme for the adaptive controller to select from
    ///
    /// @param name Name of the candidate scheme to add
    /// @param order Order of the candidate scheme
    /// @param stage_order Stage order of the candidate scheme
    /// @param ccfl Stability coefficient relative to explicit Euler, used for CFL constraints
    /// @param cost Relative measure of the amount of work required for the candidate scheme
    /// @param inuse Indicates that this scheme is the one currently in use, this flag can only be
    /// set for one scheme
    void add_candidate(const std::string & name,
                       Int order,
                       Int stage_order,
                       Real ccfl,
                       Real cost,
                       bool inuse);

    /// Clear any previously set candidate schemes
    void clear_candidates();

    std::vector<Candidate> get_candidates() const;

    /// Checks whether to accept a stage, (e.g. reject and change time step size if nonlinear
    /// solve fails or solution vector is infeasible)
    ///
    /// @param ts Time stepper
    /// @param t Current simulation time
    /// @param Y Current solution vector
    /// @return `true` to accept the stage, `false` to reject
    bool check_stage(TS ts, Real t, const Vector & Y) const;

    /// Choose which method and step size to use for the next step
    ///
    /// @param ts Time stepper
    /// @param h Current step size
    /// @return [next_sc, next_h, accept]
    /// - next_sc: scheme to use for the next step
    /// - next_h: step size to use for the next step
    /// - accept: `true` to accept the current step, `false` to repeat the current step with the
    ///   new step size
    std::tuple<Int, Real, bool> choose(TS ts, Real h);

    /// Gets the admissible decrease/increase factor in step size in the time step adapter
    ///
    /// @return [low, high]
    /// - low: admissible decrease factor,
    /// - high: admissible increase factor
    std::tuple<Real, Real> get_clip() const;

    /// Get error estimation threshold. Solution components below this threshold value will not be
    /// considered when computing error norms for time step adaptivity (in absolute value).
    ///
    /// @return Threshold for solution components that are ignored during error estimation
    Real get_max_ignore() const;

    /// Get safety factors for time step adapter
    ///
    /// @return [safety, reject_safety]
    /// - safety: safety factor relative to target error/stability goal
    /// - reject_safety: extra safety factor to apply if the last step was rejected
    std::tuple<Real, Real> get_safety() const;

    /// Gets the admissible decrease/increase factor in step size
    ///
    /// @return Scale factor
    Real get_scale_solve_failed() const;

    /// Get the minimum and maximum step sizes to be considered by the time step controller
    ///
    /// @return [hmin, hmax]
    /// - hmin: minimum time step
    /// - hmax: maximum time step
    std::tuple<Real, Real> get_step_limits() const;

    /// Get the adapter method type (as a string)
    ///
    /// @return The name of adapter method
    std::string get_type() const;

    /// Reset the time stepper to its defaults
    void reset();

    /// Set whether to always accept steps regardless of any error or stability condition not
    /// meeting the prescribed goal.
    ///
    /// @param flag Whether to always accept steps
    void set_always_accept(bool flag);

    /// Sets the admissible decrease/increase factor in step size in the time step adapter
    ///
    /// @param low Admissible decrease factor
    /// @param high Admissible increase factor
    void set_clip(Real low, Real high);

    /// Set error estimation threshold. Solution components below this threshold value will not be
    /// considered when computing error norms for time step adaptivity (in absolute value). A
    /// negative value (default) of the threshold leads to considering all solution components.
    ///
    /// @param max_ignore Threshold for solution components that are ignored during error estimation
    void set_max_ignore(Real max_ignore);

    /// Set safety factors for time step adaptor
    ///
    /// @param safety Safety factor relative to target error/stability goal
    /// @param reject_safety Extra safety factor to apply if the last step was rejected
    void set_safety(Real safety, Real reject_safety);

    /// Scale step size by this factor if solve fails
    ///
    /// @param scale Scale
    void set_scale_solve_failed(Real scale);

    /// Set the minimum and maximum step sizes to be considered by the time step controller
    ///
    /// @param hmin Minimum time step
    /// @param hmax Maximum time step
    void set_step_limits(Real hmin, Real hmax);

    /// Set the adapter method
    ///
    /// @param type Adapter method
    void set_type(const std::string & type);

    /// Set the adapter method
    void set_type(Type type);

private:
    TSAdapt tsadapt;

public:
    static TimeStepAdapt from_ts(TS ts);
};

} // namespace godzilla
