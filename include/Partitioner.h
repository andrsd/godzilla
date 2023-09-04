#pragma once

#include "petscpartitioner.h"
#include "Types.h"
#include "Section.h"
#include "IndexSet.h"

namespace godzilla {

class Partitioner {
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

    void view(PetscViewer viewer = PETSC_VIEWER_STDOUT_WORLD) const;

    operator PetscPartitioner() const;

private:
    PetscPartitioner part;
};

} // namespace godzilla
