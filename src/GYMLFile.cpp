#include "GYMLFile.h"
#include "App.h"
#include "Factory.h"
#include "Mesh.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "Function.h"
#include "AuxiliaryField.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "DiscreteProblemInterface.h"
#include "FEProblemInterface.h"
#include "TransientProblemInterface.h"
#include "TimeSteppingAdaptor.h"
#include "Postprocessor.h"
#include "CallStack.h"
#include "Validation.h"
#include "Utils.h"
#include "cassert"
#include "fmt/format.h"
#include "yaml-cpp/node/iterator.h"

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
    YAML::Node funcs_node = this->root["functions"];
    if (!funcs_node)
        return;

    lprintf(9, "- functions");

    for (const auto & it : funcs_node) {
        YAML::Node fn_node = it.first;
        auto name = fn_node.as<std::string>();

        Parameters * params = build_params(funcs_node, name);
        const auto & class_name = params->get<std::string>("_type");
        auto fn = Factory::create<Function>(class_name, name, params);
        assert(this->problem != nullptr);
        this->problem->add_function(fn);
    }
}

void
GYMLFile::build_problem_adapt()
{
    _F_;
    YAML::Node problem_node = this->root["problem"];
    if (this->problem && problem_node && problem_node["ts_adapt"])
        build_ts_adapt(problem_node);
}

void
GYMLFile::build_ts_adapt(const YAML::Node & problem_node)
{
    _F_;
    auto * tpi = dynamic_cast<TransientProblemInterface *>(this->problem);
    if (tpi != nullptr) {
        Parameters * params = build_params(problem_node, "ts_adapt");
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

    YAML::Node part_node = this->root["partitioner"];
    if (!part_node)
        return;

    lprintf(9, "- partitioner");

    YAML::Node name = part_node["name"];
    if (name)
        mesh->set_partitioner_type(name.as<std::string>());

    YAML::Node overlap = part_node["overlap"];
    if (overlap)
        mesh->set_partition_overlap(overlap.as<PetscInt>());
}

void
GYMLFile::build_auxiliary_fields()
{
    _F_;
    YAML::Node auxs_root_node = this->root["auxs"];
    if (!auxs_root_node)
        return;

    lprintf(9, "- auxiliary fields");

    auto * fepface = dynamic_cast<FEProblemInterface *>(this->problem);
    if (fepface == nullptr)
        log_error("Supplied problem type '%s' does not support auxiliary fields.",
                  this->problem->get_type());
    else {
        for (const auto & it : auxs_root_node) {
            YAML::Node aux_node = it.first;
            auto name = aux_node.as<std::string>();

            Parameters * params = build_params(auxs_root_node, name);
            params->set<FEProblemInterface *>("_fepi") = fepface;
            const auto & class_name = params->get<std::string>("_type");
            auto aux = Factory::create<AuxiliaryField>(class_name, name, params);
            fepface->add_auxiliary_field(aux);
        }
    }
}

void
GYMLFile::build_initial_conditions()
{
    _F_;
    YAML::Node ics_root_node = this->root["ics"];
    if (!ics_root_node)
        return;

    lprintf(9, "- initial conditions");

    auto * dpi = dynamic_cast<DiscreteProblemInterface *>(this->problem);
    if (dpi == nullptr)
        log_error("Supplied problem type '%s' does not support initial conditions.",
                  this->problem->get_type());
    else {
        for (const auto & it : ics_root_node) {
            YAML::Node ic_node = it.first;
            auto name = ic_node.as<std::string>();

            Parameters * params = build_params(ics_root_node, name);
            params->set<const DiscreteProblemInterface *>("_dpi") = dpi;
            const auto & class_name = params->get<std::string>("_type");
            auto ic = Factory::create<InitialCondition>(class_name, name, params);
            dpi->add_initial_condition(ic);
        }
    }
}

void
GYMLFile::build_boundary_conditions()
{
    _F_;
    YAML::Node bcs_root_node = this->root["bcs"];
    if (!bcs_root_node)
        return;

    lprintf(9, "- boundary conditions");

    auto * dpi = dynamic_cast<DiscreteProblemInterface *>(this->problem);
    if (dpi == nullptr)
        log_error("Supplied problem type '%s' does not support boundary conditions.",
                  this->problem->get_type());
    else {
        for (const auto & it : bcs_root_node) {
            YAML::Node bc_node = it.first;
            auto name = bc_node.as<std::string>();

            Parameters * params = build_params(bcs_root_node, name);
            params->set<const DiscreteProblemInterface *>("_dpi") = dpi;
            const auto & class_name = params->get<std::string>("_type");
            auto bc = Factory::create<BoundaryCondition>(class_name, name, params);
            dpi->add_boundary_condition(bc);
        }
    }
}

void
GYMLFile::build_postprocessors()
{
    _F_;
    YAML::Node pps_root_node = this->root["pps"];
    if (!pps_root_node)
        return;

    lprintf(9, "- post-processors");

    for (const auto & it : pps_root_node) {
        YAML::Node pps_node = it.first;
        auto name = pps_node.as<std::string>();

        Parameters * params = build_params(pps_root_node, name);
        const auto & class_name = params->get<std::string>("_type");
        params->set<const Problem *>("_problem") = this->problem;
        auto pp = Factory::create<Postprocessor>(class_name, name, params);
        problem->add_postprocessor(pp);
    }
}

} // namespace godzilla
