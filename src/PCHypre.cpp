// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PCHypre.h"
#include "godzilla/CallStack.h"
#include "godzilla/Error.h"

namespace godzilla {

PCHypre::PCHypre() : Preconditioner()
{
#ifdef PETSC_HAVE_HYPRE
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

PCHypre::PCHypre(PC pc) : Preconditioner(pc)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    Preconditioner::set_type(PCHYPRE);
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::create(MPI_Comm comm)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    Preconditioner::create(comm);
    Preconditioner::set_type(PCHYPRE);
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::set_type(Type type)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    if (type == EUCLID)
        PETSC_CHECK(PCHYPRESetType(this->pc, "euclid"));
    else if (type == PILUT)
        PETSC_CHECK(PCHYPRESetType(this->pc, "pilut"));
    else if (type == PARASAILS)
        PETSC_CHECK(PCHYPRESetType(this->pc, "parasails"));
    else if (type == BOOMERAMG)
        PETSC_CHECK(PCHYPRESetType(this->pc, "boomeramg"));
    else if (type == AMS)
        PETSC_CHECK(PCHYPRESetType(this->pc, "ams"));
    else if (type == ADS)
        PETSC_CHECK(PCHYPRESetType(this->pc, "ads"));
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

PCHypre::Type
PCHypre::get_type() const
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    const char * name;
    PETSC_CHECK(PCHYPREGetType(this->pc, &name));
    if (strcmp(name, "euclid") == 0)
        return EUCLID;
    else if (strcmp(name, "pilut") == 0)
        return PILUT;
    else if (strcmp(name, "parasails") == 0)
        return PARASAILS;
    else if (strcmp(name, "boomeramg") == 0)
        return BOOMERAMG;
    else if (strcmp(name, "ams") == 0)
        return AMS;
    else if (strcmp(name, "ads") == 0)
        return ADS;
    else
        throw std::logic_error("Unknown type of HYPRE preconditioner.");
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::ams_set_interior_nodes(const Vector & interior)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    #if PETSC_VERSION_GE(3, 18, 0)
    PETSC_CHECK(PCHYPREAMSSetInteriorNodes(this->pc, interior));
    #endif
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::set_alpha_poisson_matrix(const Matrix & A)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    PETSC_CHECK(PCHYPRESetAlphaPoissonMatrix(this->pc, A));
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::set_beta_poisson_matrix(const Matrix & A)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    PETSC_CHECK(PCHYPRESetBetaPoissonMatrix(this->pc, A));
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::set_discrete_curl(const Matrix & C)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    PETSC_CHECK(PCHYPRESetDiscreteCurl(this->pc, C));
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::set_discrete_gradient(const Matrix & G)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    PETSC_CHECK(PCHYPRESetDiscreteGradient(this->pc, G));
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::set_edge_constant_vectors(const Vector & oz, const Vector & zo)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    PETSC_CHECK(PCHYPRESetEdgeConstantVectors(this->pc, oz, zo, nullptr));
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

void
PCHypre::set_edge_constant_vectors(const Vector & ozz, const Vector & zoz, const Vector & zzo)
{
    CALL_STACK_MSG();
#ifdef PETSC_HAVE_HYPRE
    PETSC_CHECK(PCHYPRESetEdgeConstantVectors(this->pc, ozz, zoz, zzo));
#else
    throw Exception("PETSc was not built with HYPRE.");
#endif
}

} // namespace godzilla
