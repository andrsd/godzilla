#pragma once

#include "petsc.h"

namespace godzilla {

class Init {
public:
    Init(int argc, char * argv[]);
    virtual ~Init();
};

} // namespace godzilla
