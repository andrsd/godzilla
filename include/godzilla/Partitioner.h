// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/PetscObjectWrapper.h"
#include "godzilla/Types.h"
#include "godzilla/Section.h"
#include "godzilla/IndexSet.h"
#include "petscpartitioner.h"

namespace godzilla {

class Partitioner : public PetscObjectWrapper<PetscPartitioner> {
public:
    Partitioner();
    explicit Partitioner(MPI_Comm comm);
    explicit Partitioner(PetscPartitioner p);

    void create(MPI_Comm comm);
    void destroy();

    void set_type(const std::string & type);
    std::string get_type() const;

    void reset();
    void set_up();

    void partition(Int n_parts,
                   Int n_vertices,
                   Int start[],
                   Int adjacency[],
                   const Section & vertex_section,
                   const Section & target_section,
                   Section & partSection,
                   IndexSet & partition);
    void partition(Int n_parts,
                   Int n_vertices,
                   Int start[],
                   Int adjacency[],
                   const Section & vertex_section,
                   const Section & edge_section,
                   const Section & target_section,
                   Section & partSection,
                   IndexSet & partition);

    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;
};

} // namespace godzilla
