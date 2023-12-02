#pragma once

#include "godzilla/App.h"

using namespace godzilla;

class TestApp : public App {
public:
    TestApp() : App(mpi::Communicator(MPI_COMM_WORLD), "godzilla") {}
};
