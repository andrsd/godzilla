// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/GYMLFile.h"
#include "godzilla/App.h"
#include "godzilla/Factory.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Problem.h"
#include "godzilla/AuxiliaryField.h"
#include "godzilla/InitialCondition.h"
#include "godzilla/BoundaryCondition.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/FVProblemInterface.h"
#include "godzilla/TransientProblemInterface.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/CallStack.h"
#include "godzilla/Assert.h"

namespace godzilla {

GYMLFile::GYMLFile(App * app) : InputFile(app)
{
    CALL_STACK_MSG();
}

void
GYMLFile::build()
{
    CALL_STACK_MSG();
    lprintln(9, "Allocating objects");
    build_mesh();
    build_problem();
    build_problem_adapt();
    build_functions();
    build_partitioner();
    build_auxiliary_fields();
    build_initial_conditions();
    build_boundary_conditions();
    build_postprocessors();
    build_outputs();
}

void
GYMLFile::build_functions()
{
    CALL_STACK_MSG();
#if 0
    if (!get_root()["functions"])
        return;

    lprintln(9, "- functions");
    auto funcs_block = get_block(get_root(), "functions");
    for (const auto & it : funcs_block.values()) {
        Block blk = get_block(funcs_block, it.first.as<std::string>());
        Parameters * params = build_params(blk);
        auto fn = get_app()->build_object<Function>(blk.name(), params);
        assert_true(get_problem() != nullptr, "Problem is nullptr");
        get_problem()->add_function(fn);
    }
#endif
}

void
GYMLFile::build_problem_adapt()
{
    CALL_STACK_MSG();
    if (get_problem()) {
        auto problem_node = get_block(get_root(), "problem");
        if (problem_node["ts_adapt"])
            build_ts_adapt(problem_node);
    }
}

void
GYMLFile::build_ts_adapt(const Block & problem_node)
{
    CALL_STACK_MSG();
    auto * tpi = dynamic_cast<TransientProblemInterface *>(get_problem());
    if (tpi != nullptr) {
        auto ts_adapt_node = get_block(problem_node, "ts_adapt");
        auto * params = build_params(ts_adapt_node);
        params->set<Problem *>("_problem", get_problem());

#if 0
        auto * ts_adaptor = get_app()->build_object<TimeSteppingAdaptor>("ts_adapt", params);
        tpi->set_time_stepping_adaptor(ts_adaptor);
#endif
    }
    else
        log_error("Time stepping adaptivity can be used only with transient problems.");
}

void
GYMLFile::build_partitioner()
{
    CALL_STACK_MSG();
    auto prob = get_problem();
    if (!prob)
        return;

    if (!get_root()["partitioner"])
        return;

    lprintln(9, "- partitioner");
    auto part_node = get_block(get_root(), "partitioner");
    auto name = part_node["name"];
    if (name)
        prob->set_partitioner_type(name.as<std::string>());

    auto overlap = part_node["overlap"];
    if (overlap)
        prob->set_partition_overlap(overlap.as<Int>());
}

void
GYMLFile::build_auxiliary_fields()
{
    CALL_STACK_MSG();
    if (!get_root()["auxs"])
        return;

#if 0
    lprintln(9, "- auxiliary fields");
    auto auxs_node = get_block(get_root(), "auxs");
    auto * fepi = dynamic_cast<FEProblemInterface *>(get_problem());
    auto * fvpi = dynamic_cast<FVProblemInterface *>(get_problem());
    if (fepi != nullptr || fvpi != nullptr) {
        auto * dpi = dynamic_cast<DiscreteProblemInterface *>(get_problem());
        for (const auto & it : auxs_node.values()) {
            Block blk = get_block(auxs_node, it.first.as<std::string>());
            auto * params = build_params(blk);
            params->set<DiscreteProblemInterface *>("_dpi", dpi);
            auto aux = get_app()->build_object<AuxiliaryField>(blk.name(), params);
            dpi->add_auxiliary_field(aux);
        }
    }
    else
        log_error("Supplied problem type '{}' does not support auxiliary fields.",
                  get_problem()->get_type());
#endif
}

void
GYMLFile::build_initial_conditions()
{
    CALL_STACK_MSG();
#if 0
    if (!get_root()["ics"])
        return;

    lprintln(9, "- initial conditions");
    auto ics_node = get_block(get_root(), "ics");
    auto * dpi = dynamic_cast<DiscreteProblemInterface *>(get_problem());
    if (dpi == nullptr)
        log_error("Supplied problem type '{}' does not support initial conditions.",
                  get_problem()->get_type());
    else {
        for (const auto & it : ics_node.values()) {
            Block blk = get_block(ics_node, it.first.as<std::string>());
            auto * params = build_params(blk);
            params->set<DiscreteProblemInterface *>("_dpi", dpi);
            auto ic = get_app()->build_object<InitialCondition>(blk.name(), params);
            dpi->add_initial_condition(ic);
        }
    }
#endif
}

void
GYMLFile::build_boundary_conditions()
{
    CALL_STACK_MSG();
#if 0
    if (!get_root()["bcs"])
        return;

    lprintln(9, "- boundary conditions");
    auto bcs_node = get_block(get_root(), "bcs");
    auto * dpi = dynamic_cast<DiscreteProblemInterface *>(get_problem());
    if (dpi == nullptr)
        log_error("Supplied problem type '{}' does not support boundary conditions.",
                  get_problem()->get_type());
    else {
        for (const auto & it : bcs_node.values()) {
            Block blk = get_block(bcs_node, it.first.as<std::string>());
            auto * params = build_params(blk);
            params->set<DiscreteProblemInterface *>("_dpi", dpi);
            auto bc = get_app()->build_object<BoundaryCondition>(blk.name(), params);
            dpi->add_boundary_condition(bc);
        }
    }
#endif
}

void
GYMLFile::build_postprocessors()
{
    CALL_STACK_MSG();
    if (!get_root()["pps"])
        return;

    lprintln(9, "- post-processors");
    auto pps_node = get_block(get_root(), "pps");
    for (const auto & it : pps_node.values()) {
        Block blk = get_block(pps_node, it.first.as<std::string>());
        auto * params = build_params(blk);
        params->set<Problem *>("_problem", get_problem());
        auto pp = get_app()->build_object<Postprocessor>(blk.name(), params);
        get_problem()->add_postprocessor(pp);
    }
}

} // namespace godzilla
