#pragma once

#include <mpi.h>

class GodzillaInit
{
public:
    GodzillaInit(int argc, char * argv[], MPI_Comm COMM_WORLD_IN = MPI_COMM_WORLD);
    virtual ~GodzillaInit();
};
