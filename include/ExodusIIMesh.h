#pragma once

#include <string>
#include "Mesh.h"

namespace godzilla {

/// Mesh that can be loaded from ExodusII file
///
class ExodusIIMesh : public Mesh {
public:
    ExodusIIMesh(const InputParameters & params);

    const std::string & get_file_name() const;

protected:
    virtual void create_dm() override;

    /// File name with the ExodusII mesh
    const std::string & file_name;

public:
    static InputParameters validParams();
};

} // namespace godzilla
