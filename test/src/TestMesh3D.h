#pragma once

#include "godzilla/Types.h"
#include "godzilla/Label.h"
#include "godzilla/UnstructuredMesh.h"
#include "godzilla/MeshObject.h"

class TestMesh3D : public godzilla::MeshObject {
public:
    explicit TestMesh3D(const godzilla::Parameters & parameters);

    godzilla::Mesh * create_mesh() override;

    void create_side_set(godzilla::UnstructuredMesh * mesh,
                         godzilla::Label & face_sets,
                         godzilla::Int id,
                         const std::vector<godzilla::Int> & faces,
                         const char * name);

public:
    static godzilla::Parameters parameters();
};
