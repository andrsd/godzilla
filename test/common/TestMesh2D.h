#pragma once

#include "godzilla/Qtr.h"
#include "godzilla/Types.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Object.h"

class TestMesh2D : public godzilla::Object {
public:
    explicit TestMesh2D(const godzilla::Parameters & pars);

    godzilla::Qtr<godzilla::UnstructuredMesh> create_mesh();

    void create_side_set(godzilla::Qtr<godzilla::UnstructuredMesh> & mesh,
                         godzilla::Label & face_sets,
                         godzilla::Int id,
                         const std::vector<godzilla::Int> & faces,
                         const char * name);

public:
    static godzilla::Parameters parameters();
};
