#include "GYMLFile.h"
#include "App.h"
#include "Factory.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "Function.h"
#include "AuxiliaryField.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "DiscreteProblemInterface.h"
#include "FEProblemInterface.h"
#include "FVProblemInterface.h"
#include "TransientProblemInterface.h"
#include "TimeSteppingAdaptor.h"
#include "Postprocessor.h"
#include "CallStack.h"
#include <cassert>

namespace godzilla {

GYMLFile::GYMLFile(const App * app) : InputFile(app)
{
    _F_;
}

void
GYMLFile::build()
{
    _F_;
    lprintf(9, "Allocating objects");
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
    _F_;
    if (!this->root["functions"])
        return;

    lprintf(9, "- functions");
    auto funcs_block = get_block(this->root, "functions");
    for (const auto & it : funcs_block.values()) {
        Block blk = get_block(funcs_block, it.first.as<std::string>());
        Parameters * params = build_params(blk);
        const auto & class_name = params->get<std::string>("_type");
        auto fn = Factory::create<Function>(class_name, blk.name(), params);
        assert(this->problem != nullptr);
        this->problem->add_function(fn);
    }
}

void
GYMLFile::build_problem_adapt()
{
    _F_;
    if (this->problem) {
        auto problem_node = get_block(this->root, "problem");
        if (problem_node["ts_adapt"])
            build_ts_adapt(problem_node);
    }
}

void
GYMLFile::build_ts_adapt(const Block & problem_node)
{
    _F_;
    auto * tpi = dynamic_cast<TransientProblemInterface *>(this->problem);
    if (tpi != nullptr) {
        auto ts_adapt_node = get_block(problem_node, "ts_adapt");
        Parameters * params = build_params(ts_adapt_node);
        params->set<const Problem *>("_problem") = this->problem;
        params->set<const TransientProblemInterface *>("_tpi") = tpi;

        const auto & class_name = params->get<std::string>("_type");
        auto * ts_adaptor = Factory::create<TimeSteppingAdaptor>(class_name, "ts_adapt", params);
        tpi->set_time_stepping_adaptor(ts_adaptor);
    }
    else
        log_error("Time stepping adaptivity can be used only with transient problems.");
}

void
GYMLFile::build_partitioner()
{
    _F_;
    if (!this->mesh)
        return;

    auto * mesh = dynamic_cast<UnstructuredMesh *>(this->mesh);
    if (!mesh)
        return;

    if (!this->root["partitioner"])
        return;

    lprintf(9, "- partitioner");
    auto part_node = get_block(this->root, "partitioner");
    auto name = part_node["name"];
    if (name)
        mesh->set_partitioner_type(name.as<std::string>());

    auto overlap = part_node["overlap"];
    if (overlap)
        mesh->set_partition_overlap(overlap.as<Int>());
}

void
GYMLFile::build_auxiliary_fields()
{
    _F_;
    if (!this->root["auxs"])
        return;

    lprintf(9, "- auxiliary fields");
    auto auxs_node = get_block(this->root, "auxs");
    auto * fepi = dynamic_cast<FEProblemInterface *>(this->problem);
    auto * fvpi = dynamic_cast<FVProblemInterface *>(this->problem);
    if (fepi != nullptr || fvpi != nullptr) {
        auto * dpi = dynamic_cast<DiscreteProblemInterface *>(this->problem);
        for (const auto & it : auxs_node.values()) {
            Block blk = get_block(auxs_node, it.first.as<std::string>());
            Parameters * params = build_params(blk);
            params->set<DiscreteProblemInterface *>("_dpi") = dpi;
            const auto & class_name = params->get<std::string>("_type");
            auto aux = Factory::create<AuxiliaryField>(class_name, blk.name(), params);
            dpi->add_auxiliary_field(aux);
        }
    }
    else
        log_error("Supplied problem type '{}' does not support auxiliary fields.",
                  this->problem->get_type());
}

void
GYMLFile::build_initial_conditions()
{
    _F_;
    if (!this->root["ics"])
        return;

    lprintf(9, "- initial conditions");
    auto ics_node = get_block(this->root, "ics");
    auto * dpi = dynamic_cast<DiscreteProblemInterface *>(this->problem);
    if (dpi == nullptr)
        log_error("Supplied problem type '{}' does not support initial conditions.",
                  this->problem->get_type());
    else {
        for (const auto & it : ics_node.values()) {
            Block blk = get_block(ics_node, it.first.as<std::string>());
            Parameters * params = build_params(blk);
            params->set<const DiscreteProblemInterface *>("_dpi") = dpi;
            const auto & class_name = params->get<std::string>("_type");
            auto ic = Factory::create<InitialCondition>(class_name, blk.name(), params);
            dpi->add_initial_condition(ic);
        }
    }
}

void
GYMLFile::build_boundary_conditions()
{
    _F_;
    if (!this->root["bcs"])
        return;

    lprintf(9, "- boundary conditions");
    auto bcs_node = get_block(this->root, "bcs");
    auto * dpi = dynamic_cast<DiscreteProblemInterface *>(this->problem);
    if (dpi == nullptr)
        log_error("Supplied problem type '{}' does not support boundary conditions.",
                  this->problem->get_type());
    else {
        for (const auto & it : bcs_node.values()) {
            Block blk = get_block(bcs_node, it.first.as<std::string>());
            Parameters * params = build_params(blk);
            params->set<const DiscreteProblemInterface *>("_dpi") = dpi;
            const auto & class_name = params->get<std::string>("_type");
            auto bc = Factory::create<BoundaryCondition>(class_name, blk.name(), params);
            dpi->add_boundary_condition(bc);
        }
    }
}

void
GYMLFile::build_postprocessors()
{
    _F_;
    if (!this->root["pps"])
        return;

    lprintf(9, "- post-processors");
    auto pps_node = get_block(this->root, "pps");
    for (const auto & it : pps_node.values()) {
        Block blk = get_block(pps_node, it.first.as<std::string>());
        Parameters * params = build_params(blk);
        const auto & class_name = params->get<std::string>("_type");
        params->set<const Problem *>("_problem") = this->problem;
        auto pp = Factory::create<Postprocessor>(class_name, blk.name(), params);
        problem->add_postprocessor(pp);
    }
}

} // namespace godzilla
