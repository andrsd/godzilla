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

    virtual void create() override;

protected:
    void load(const std::string & file_name);

    void load_coordinates(int n_nodes);

    void load_block(int blk_id, int & elem_id);

    /// File name with the ExodusII mesh
    const std::string & file_name;
    /// ExodusII file handle
    int exoid;

public:
    static InputParameters validParams();
};

} // namespace godzilla
