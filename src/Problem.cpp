// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Problem.h"
#include "godzilla/CallStack.h"
#include "godzilla/MeshObject.h"
#include "godzilla/Mesh.h"
#include "godzilla/Function.h"
#include "godzilla/Postprocessor.h"
#include "godzilla/Output.h"
#include "godzilla/FileOutput.h"
#include "godzilla/Section.h"

namespace godzilla {

Problem::FieldDecomposition::FieldDecomposition(Int n) : field_name(n), is(n) {}

Int
Problem::FieldDecomposition::get_num_fields() const
{
    return static_cast<Int>(this->field_name.size());
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
    set_output_monitor(this, &Problem::output_monitor);
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
    assert(this->mesh != nullptr);
    auto m = this->mesh->get_mesh<Mesh>();
    assert(m != nullptr);
    return m->get_dm();
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

std::string
Problem::get_vector_type() const
{
    CALL_STACK_MSG();
    VecType type;
    PETSC_CHECK(DMGetVecType(get_dm(), &type));
    return std::string { type };
}

void
Problem::set_vector_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetVecType(get_dm(), type.c_str()));
}

void
Problem::create()
{
    CALL_STACK_MSG();
    for (auto & f : this->functions)
        f->create();
    for (auto & pp : this->pps)
        pp.second->create();
    for (auto & out : this->outputs)
        out->create();
}

void
Problem::set_up_types()
{
    CALL_STACK_MSG();
    // do nothing, i.e. use PETSc defaults
}

void
Problem::allocate_objects()
{
    CALL_STACK_MSG();
    set_solution_vector(create_global_vector());
}

Int
Problem::get_dimension() const
{
    CALL_STACK_MSG();
    Int dim;
    PETSC_CHECK(DMGetDimension(get_dm(), &dim));
    return dim;
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
    auto fo = dynamic_cast<FileOutput *>(output);
    if (fo)
        this->file_outputs.push_back(fo);
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
    for (auto & o : this->file_outputs)
        if (o->should_output(flag)) {
            if (this->output_monitor_delegate)
                this->output_monitor_delegate(o->get_file_name());
            o->output_step();
        }
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

void
Problem::set_solution_vector(const Vector & x)
{
    CALL_STACK_MSG();
    this->x = x;
    this->x.set_name("sln");
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

std::string
Problem::get_matrix_type() const
{
    CALL_STACK_MSG();
    MatType type;
    PETSC_CHECK(DMGetMatType(get_dm(), &type));
    return std::string { type };
}

void
Problem::set_matrix_type(const std::string & type)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetMatType(get_dm(), type.c_str()));
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
    for (Int i = 0; i < n; ++i) {
        decomp.field_name[i] = field_names[i];
        decomp.is[i] = IndexSet(is[i]);
    }
    for (Int i = 0; i < n; ++i)
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

Int
Problem::get_num_auxiliary_vec() const
{
    CALL_STACK_MSG();
    Int n;
    PETSC_CHECK(DMGetNumAuxiliaryVec(get_dm(), &n));
    return n;
}

Vector
Problem::get_auxiliary_vec(const Label & label, Int value, Int part) const
{
    CALL_STACK_MSG();
    Vec vec;
    PETSC_CHECK(DMGetAuxiliaryVec(get_dm(), label, value, part, &vec));
    return { vec };
}

void
Problem::set_auxiliary_vec(const Label & label, Int value, Int part, const Vector & vec)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetAuxiliaryVec(get_dm(), label, value, part, vec));
}

void
Problem::clear_auxiliary_vec()
{
#if PETSC_VERSION_GE(3, 21, 0)
    CALL_STACK_MSG();
    PETSC_CHECK(DMClearAuxiliaryVec(get_dm()));
#else
    throw Exception("You need PETSC 3.21+ for `Problem::clear_auxiliary_vec()`");
#endif
}

IndexSet
Problem::create_section_subis(const std::vector<Int> & fields) const
{
    CALL_STACK_MSG();
#if PETSC_VERSION_GE(3, 21, 0)
    IS is;
    PETSC_CHECK(
        DMCreateSectionSubDM(get_dm(), fields.size(), fields.data(), NULL, NULL, &is, NULL));
    return IndexSet(is);
#else
    IS is;
    PETSC_CHECK(DMCreateSectionSubDM(get_dm(), fields.size(), fields.data(), &is, NULL));
    return IndexSet(is);
#endif
}

IndexSet
Problem::create_section_subis(const std::vector<Int> & fields,
                              const std::vector<Int> & n_comps,
                              const std::vector<Int> & comps) const
{
    CALL_STACK_MSG();
#if PETSC_VERSION_GE(3, 21, 0)
    assert(fields.size() == n_comps.size());
    IS is;
    PETSC_CHECK(DMCreateSectionSubDM(get_dm(),
                                     fields.size(),
                                     fields.data(),
                                     n_comps.data(),
                                     comps.data(),
                                     &is,
                                     NULL));
    return IndexSet(is);
#else
    throw Exception(
        "PETSc 3.21+ is needed for Problem::create_section_subis with component support");
#endif
}

void
Problem::output_monitor(const std::string & file_name) const
{
    lprintln(9, "Output to file: {}", file_name);
}

void
Problem::set_adjacency(Int field, bool use_cone, bool use_closure)
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMSetAdjacency(get_dm(),
                               field,
                               use_cone ? PETSC_TRUE : PETSC_FALSE,
                               use_closure ? PETSC_TRUE : PETSC_FALSE));
}

void
Problem::local_to_global(const Vector & l, InsertMode mode, Vector & g) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMLocalToGlobal(get_dm(), l, mode, g));
}

void
Problem::global_to_local(const Vector & g, InsertMode mode, Vector & l) const
{
    CALL_STACK_MSG();
    PETSC_CHECK(DMGlobalToLocal(get_dm(), g, mode, l));
}

} // namespace godzilla
