#pragma once

#include "godzilla/Qtr.h"
#include "godzilla/Types.h"
#include "godzilla/Object.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/Label.h"

class TestMesh1D : public godzilla::Object {
public:
    explicit TestMesh1D(const godzilla::Parameters & pars);

    godzilla::Qtr<godzilla::UnstructuredMesh> create_mesh();

    void create_side_set(godzilla::Qtr<godzilla::UnstructuredMesh> & mesh,
                         godzilla::Label & face_sets,
                         godzilla::Int id,
                         const std::vector<godzilla::Int> & faces,
                         const char * name);

public:
    static godzilla::Parameters parameters();
};
