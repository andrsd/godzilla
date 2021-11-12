#pragma once

#include "mpi.h"

namespace godzilla {

class Init {
public:
    Init(int argc, char * argv[], MPI_Comm COMM_WORLD_IN = MPI_COMM_WORLD);
    virtual ~Init();
};

} // namespace godzilla
