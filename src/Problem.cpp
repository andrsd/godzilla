// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Problem.h"
#include "godzilla/CallStack.h"
#include "godzilla/MeshObject.h"
#include "godzilla/Mesh.h"
#include "godzilla/Function.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/Output.h"
#include "godzilla/Section.h"

namespace godzilla {

Problem::FieldDecomposition::FieldDecomposition(Int n) : field_name(n), is(n) {}

Int
Problem::FieldDecomposition::get_num_fields() const
{
    return this->field_name.size();
}

void
Problem::FieldDecomposition::destroy()
{
    for (auto & e : this->is)
        e.destroy();
    this->field_name.clear();
    this->is.clear();
}

Parameters
Problem::parameters()
{
    Parameters params = Object::parameters();
    params.add_private_param<MeshObject *>("_mesh_obj", nullptr);
    return params;
}

Problem::Problem(const Parameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    mesh(get_param<MeshObject *>("_mesh_obj")),
    partitioner(nullptr),
    partition_overlap(0),
    default_output_on()
{
    this->partitioner.create(get_comm());
}

Problem::~Problem()
{
    this->x.destroy();
    this->partitioner.destroy();
}

DM
Problem::get_dm() const
{
    CALL_STACK_MSG();
    return get_mesh()->get_dm();
}

const Vector &
Problem::get_solution_vector() const
{
    CALL_STACK_MSG();
    return this->x;
}

Vector &
Problem::get_solution_vector()
{
    return this->x;
}

void
Problem::create()
{
    CALL_STACK_MSG();
    this->partitioner.create(get_comm());
    for (auto & f : this->functions)
        f->create();
    for (auto & pp : this->pps)
        pp.second->create();
    for (auto & out : this->outputs)
        out->create();
}

void
Problem::allocate_objects()
{
    this->x = create_global_vector();
    this->x.set_name("sln");
}

Mesh *
Problem::get_mesh() const
{
    CALL_STACK_MSG();
    return this->mesh->get_mesh<Mesh>();
}

Int
Problem::get_dimension() const
{
    CALL_STACK_MSG();
    return get_mesh()->get_dimension();
}

Real
Problem::get_time() const
{
    CALL_STACK_MSG();
    return 0.;
}

Int
Problem::get_step_num() const
{
    CALL_STACK_MSG();
    return 0;
}

const std::vector<Function *> &
Problem::get_functions() const
{
    CALL_STACK_MSG();
    return this->functions;
}

void
Problem::add_function(Function * fn)
{
    CALL_STACK_MSG();
    this->functions.push_back(fn);
}

void
Problem::add_output(Output * output)
{
    CALL_STACK_MSG();
    if (!output->is_param_valid("on"))
        output->set_exec_mask(this->default_output_on);
    this->outputs.push_back(output);
}

void
Problem::add_postprocessor(Postprocessor * pp)
{
    CALL_STACK_MSG();
    this->pps_names.push_back(pp->get_name());
    this->pps[pp->get_name()] = pp;
}

void
Problem::compute_postprocessors()
{
    CALL_STACK_MSG();
    for (auto & pp : this->pps)
        pp.second->compute();
}

Postprocessor *
Problem::get_postprocessor(const std::string & name) const
{
    CALL_STACK_MSG();
    const auto & it = this->pps.find(name);
    if (it != this->pps.end())
        return it->second;
    else
        return nullptr;
}

const std::vector<std::string> &
Problem::get_postprocessor_names() const
{
    CALL_STACK_MSG();
    return this->pps_names;
}

void
Problem::output(ExecuteOnFlag flag)
{
    CALL_STACK_MSG();
    for (auto & o : this->outputs)
        if (o->should_output(flag))
            o->output_step();
}

void
Problem::on_initial()
{
    CALL_STACK_MSG();
    compute_postprocessors();
    output(EXECUTE_ON_INITIAL);
}

void
Problem::on_final()
{
    CALL_STACK_MSG();
    compute_postprocessors();
    output(EXECUTE_ON_FINAL);
}

Vector
Problem::create_local_vector() const
{
    CALL_STACK_MSG();
    Vec v;
    PETSC_CHECK(DMCreateLocalVector(get_dm(), &v));
    return { v };
}

Vector
Problem::get_local_vector() const
{
    CALL_STACK_MSG();
    Vec v;
    PETSC_CHECK(DMGetLocalVector(get_dm(), &v));
    return { v };
}

void
Problem::restore_local_vector(const Vector & vec) const
{
    CALL_STACK_MSG();
    Vec v = vec;
    PETSC_CHECK(DMRestoreLocalVector(get_dm(), &v));
}

Vector
Problem::create_global_vector() const
{
    CALL_STACK_MSG();
    Vec v;
    PETSC_CHECK(DMCreateGlobalVector(get_dm(), &v));
    return { v };
}

Vector
Problem::get_global_vector() const
{
    CALL_STACK_MSG();
    Vec v;
    PETSC_CHECK(DMGetGlobalVector(get_dm(), &v));
    return { v };
}

void
Problem::restore_global_vector(const Vector & vec) const
{
    CALL_STACK_MSG();
    Vec v = vec;
    PETSC_CHECK(DMRestoreGlobalVector(get_dm(), &v));
}

Matrix
Problem::create_matrix() const
{
    CALL_STACK_MSG();
    Mat m;
    PETSC_CHECK(DMCreateMatrix(get_dm(), &m));
    return { m };
}

Section
Problem::get_local_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetLocalSection(get_dm(), &section));
    return { section };
}

void
Problem::set_local_section(const Section & section) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetLocalSection(get_dm(), section));
}

Section
Problem::get_global_section() const
{
    PetscSection section = nullptr;
    PETSC_CHECK(DMGetGlobalSection(get_dm(), &section));
    return { section };
}

void
Problem::set_global_section(const Section & section) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetGlobalSection(get_dm(), section));
}

void
Problem::set_default_output_on(ExecuteOn flags)
{
    this->default_output_on = flags;
}

Problem::FieldDecomposition
Problem::create_field_decomposition()
{
    CALL_STACK_MSG();
    Int n;
    char ** field_names;
    IS * is;
    PETSC_CHECK(DMCreateFieldDecomposition(get_dm(), &n, &field_names, &is, nullptr));
    FieldDecomposition decomp(n);
    for (PetscInt i = 0; i < n; i++) {
        decomp.field_name[i] = field_names[i];
        decomp.is[i] = IndexSet(is[i]);
    }
    for (PetscInt i = 0; i < n; i++)
        PetscFree(field_names[i]);
    PetscFree(field_names);
    PetscFree(is);
    return decomp;
}

const Partitioner &
Problem::get_partitioner()
{
    CALL_STACK_MSG();
    return this->partitioner;
}

void
Problem::set_partitioner_type(const std::string & type)
{
    CALL_STACK_MSG();
    this->partitioner.set_type(type);
}

Int
Problem::get_partition_overlap()
{
    CALL_STACK_MSG();
    return this->partition_overlap;
}

void
Problem::set_partition_overlap(Int overlap)
{
    CALL_STACK_MSG();
    this->partition_overlap = overlap;
}

} // namespace godzilla
