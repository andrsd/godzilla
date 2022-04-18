#pragma once

#include <string>
#include "Mesh.h"

namespace godzilla {

/// ExodusII input/output
///
class ExodusIO {
public:
    static Mesh load(const std::string & file_name);

protected:
};

} // namespace godzilla
