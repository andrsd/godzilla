#pragma once

#include "godzilla/FileMesh.h"

namespace godzilla {

/// Mesh loaded from a ExodusII file
///
class ExodusIIMesh : public FileMesh {
public:
    explicit ExodusIIMesh(const Parameters & parameters);

    void create() override;

protected:
    DM create_dm() override;

public:
    static Parameters parameters();
};

} // namespace godzilla
